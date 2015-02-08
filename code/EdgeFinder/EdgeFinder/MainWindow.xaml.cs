using System.Net;
using Microsoft.CSharp;
using System.CodeDom.Compiler;
using System;
using System.Reflection;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Extensions;

namespace EdgeFinder
{

    public struct SensorCoordinates
    {
        public double LED_x;
        public double LED_y;
        public double edge_0;
        public double edge_1;
    }

    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        double widthA = 0;
        double widthB = 0;

        private Point LEDA
        {
            get
            {
                var x = (Canvas.GetLeft(led0) + led0.Width / 2.0);
                var y = (Canvas.GetTop(led0) + led0.Height / 2.0);
                return new Point(x, y);
            }
        }

        private Point LEDB
        {
            get
            {
                var x = Canvas.GetLeft(led1) + led1.Width / 2.0;
                var y = Canvas.GetTop(led1) + led1.Height / 2.0;
                return new Point(x, y);
            }
        }

        private Point EdgeA_0
        {
            get { return new Point(Canvas.GetLeft(sensor0), Canvas.GetTop(sensor0)); }
        }
        private Point EdgeA_1
        {
            get { return new Point(Canvas.GetLeft(sensor0), Canvas.GetTop(sensor0) + sensor0.Height); }
        }

        private Point EdgeB_0
        {
            get { return new Point(Canvas.GetLeft(sensor1), Canvas.GetTop(sensor1) + sensor1.Height); }
        }
        private Point EdgeB_1
        {
            get { return new Point(Canvas.GetLeft(sensor1) + sensor1.Width, Canvas.GetTop(sensor1) + sensor1.Height); }
        }

        private Point EdgeADetected_0 { get; set; }
        private Point EdgeADetected_1 { get; set; }
        private Point EdgeBDetected_0 { get; set; }
        private Point EdgeBDetected_1 { get; set; }

        private Point Filament
        {
            get
            {
                double x = Canvas.GetLeft(filament) + filament.Width / 2.0;
                double y = Canvas.GetTop(filament) + filament.Height / 2.0;
                return new Point(x, y);
            }
        }

        const double DIST_MULT = 10.0;
        BrushConverter converter = new System.Windows.Media.BrushConverter();
        Brush ledColor, validColor, errorColor;

        // Variables for drag and drop detection
        bool captured = false;
        double x_shape, x_canvas, y_shape, y_canvas;
        UIElement source = null;

        bool deltaXAllowed = true;
        bool deltaYAllowed = true;

        private void InitBrushes()
        {
            errorColor = (Brush)converter.ConvertFromString("#8d4b84");
            validColor = (Brush)converter.ConvertFromString("#4b8d54");
            ledColor = (Brush)converter.ConvertFromString("#c71153");
        }

        public MainWindow()
        {
            InitializeComponent();
            InitBrushes();
            UpdateDistances();
            RunUserCode();
        }

        private void UpdateDistances()
        {
            // Distances 0
            var df = Point.Subtract(Filament, LEDA);
            double ddf = Math.Sqrt(df.X * df.X + df.Y * df.Y) / DIST_MULT;
            var dt = Point.Subtract(EdgeA_0, LEDA);
            dt.Y += sensor0.Height / 2.0;
            double ddt = Math.Sqrt(dt.X * dt.X + dt.Y * dt.Y) / DIST_MULT;
            var dist0Text = string.Format("{0:F2} mm", ddf);
            var dist0TotalText = string.Format("{0:F2} mm", ddt);
            distances0Text.Text = dist0Text;
            totalDistance0Text.Text = dist0TotalText;

            //// Distances 1
            df = Point.Subtract(Filament, LEDB);
            ddf = Math.Sqrt(df.X * df.X + df.Y * df.Y) / DIST_MULT;
            dt = Point.Subtract(EdgeB_0, LEDB);
            dt.Y += sensor1.Width / 2.0;
            ddt = Math.Sqrt(dt.X * dt.X + dt.Y * dt.Y) / DIST_MULT;
            var dist1Text = string.Format("{0:F2} mm", ddf);
            var dist1TotalText = string.Format("{0:F2} mm", ddt);
            distances1Text.Text = dist1Text;
            totalDistance1Text.Text = dist1TotalText;

            //// Edges 0
            var edgeText = string.Format("({0:F3}, {1:F3})", EdgeADetected_0.Y, EdgeADetected_1.Y);
            var widthText = string.Format("{0:F3} mm", widthA / DIST_MULT);
            edges0Text.Text = edgeText;
            width0Text.Text = widthText;

            //// Edges 1
            edgeText = string.Format("({0:F3}, {1:F3})", EdgeBDetected_0.X, EdgeBDetected_1.X);
            widthText = string.Format("{0:F3} mm", widthB / DIST_MULT);
            edges1Text.Text = edgeText;
            width1Text.Text = widthText;

            //// X Offset
            double xOffset = (Canvas.GetLeft(sensor1) - (Canvas.GetLeft(sensor0) - sensor1.Width)) / DIST_MULT;
            xOffsetText.Text = string.Format("{0:F2} mm", xOffset);

            //// Y Offset
            double yOffset = (Canvas.GetTop(sensor1) - (Canvas.GetTop(sensor0) - sensor1.Height)) / DIST_MULT;
            yOffsetText.Text = string.Format("{0:F2} mm", yOffset);

        }
        private void shape_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            source = (UIElement)sender;
            Mouse.Capture(source);
            captured = true;
            x_shape = Canvas.GetLeft(source);
            x_canvas = e.GetPosition(LayoutRoot).X;
            y_shape = Canvas.GetTop(source);
            y_canvas = e.GetPosition(LayoutRoot).Y;
        }

        private void shape_MouseMove(object sender, MouseEventArgs e)
        {
            if (captured)
            {
                double x = e.GetPosition(LayoutRoot).X;
                double y = e.GetPosition(LayoutRoot).Y;
                x_shape += x - x_canvas;
                if (deltaXAllowed) Canvas.SetLeft(source, x_shape);
                x_canvas = x;
                y_shape += y - y_canvas;
                if (deltaYAllowed) Canvas.SetTop(source, y_shape);
                y_canvas = y;
                RunUserCode();

                ClearOldRays();
                RayCastSensorA();
                RayCastSensorB();
                UpdateDistances();
            }
        }

        private void shape_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            Mouse.Capture(null);
            captured = false;
        }

        private void keyDown(object sender, KeyEventArgs e)
        {

            if (e.Key == Key.Z)
            {
                deltaXAllowed = false;
            }
            else if (e.Key == Key.X)
            {
                deltaYAllowed = false;
            }
        }

        private void keyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Z)
            {
                deltaXAllowed = true;
            }
            else if (e.Key == Key.X)
            {
                deltaYAllowed = true;
            }
        }

        private void ClearOldRays()
        {
            var lines = LayoutRoot.Children.OfType<Line>().ToList();
            for (int i = 0; i < lines.Count; i++)
            {
                LayoutRoot.Children.Remove(lines[i]);
            }
        }

        private void RayCastSensorA()
        {
            int width = 2;
            List<int> target_points = Enumerable.Range((int)EdgeA_0.Y + width, (int)EdgeA_1.Y - (int)EdgeA_0.Y - width - 1).ToList();

            bool previousIntersection = false;
            double falling_edge = Double.NaN;
            double rising_edge = Double.NaN;

            foreach (var t in target_points)
            {
                var target = new Point(EdgeA_0.X, t);
                var dx = target.X - LEDA.X;
                var dy = target.Y - LEDA.Y;
                var slope = dy / dx;
                var points = new List<Point>();

                for (double x = LEDA.X; x < EdgeA_0.X; x++)
                {
                    double y = slope * (x - LEDA.X) + LEDA.Y;
                    points.Add(new Point(x, y));
                }

                bool intersects_filament = false;
                foreach (var point in points)
                {
                    if (Graphics.CircleContainsPoint(filament, point))
                    {
                        intersects_filament = true;
                        var ray = new Line();
                        ray.X1 = LEDA.X;
                        ray.Y1 = LEDA.Y;
                        ray.X2 = point.X;
                        ray.Y2 = point.Y;
                        // Set Line's width and color
                        SolidColorBrush redBrush = new SolidColorBrush();
                        redBrush.Color = Colors.Red;
                        redBrush.Opacity = 0.1;
                        ray.StrokeThickness = width * 2;
                        ray.Stroke = redBrush;
                        LayoutRoot.Children.Add(ray);
                        break;
                    }
                }

                // Falling edge detection
                if (!previousIntersection && intersects_filament)
                {
                    falling_edge = t;
                    EdgeADetected_0 = new Point(EdgeA_0.X, falling_edge);
                }
                // Rising edge detection
                if (!intersects_filament && previousIntersection)
                {
                    rising_edge = t - 1;
                    EdgeADetected_1 = new Point(EdgeA_0.X, rising_edge);
                }

                if (!intersects_filament)
                {
                    SolidColorBrush redBrush = new SolidColorBrush();
                    redBrush.Color = Colors.Red;
                    var ray = new Line();
                    ray.X1 = LEDA.X;
                    ray.Y1 = LEDA.Y;
                    ray.X2 = EdgeA_0.X;
                    ray.Y2 = t;
                    // Set Line's width and color
                    ray.StrokeThickness = width * 2;
                    redBrush.Opacity = 0.1;
                    ray.Stroke = redBrush;
                    LayoutRoot.Children.Add(ray);
                }
                previousIntersection = intersects_filament;

            }

            if (!Double.IsNaN(rising_edge) && !Double.IsNaN(falling_edge))
            {
                widthA = (rising_edge - falling_edge);
            }
        }

        private void RayCastSensorB()
        {
            bool previousIntersection = false;
            double falling_edge = Double.NaN;
            double rising_edge = Double.NaN;

            int width = 2;
            List<int> target_points = Enumerable.Range((int)EdgeB_0.X + width, (int)EdgeB_1.X - (int)EdgeB_0.X - width - 1).ToList();
            foreach (var t in target_points)
            {
                var target = new Point(t, EdgeB_0.Y);
                var dx = target.X - LEDB.X;
                var dy = target.Y - LEDB.Y;
                var slope = dx / dy;
                var points = new List<Point>();

                for (double y = LEDB.Y; y > EdgeB_0.Y; y--)
                {
                    double x = (slope * (y - LEDB.Y) + LEDB.X);
                    points.Add(new Point(x, y));
                }

                bool intersects_filament = false;
                foreach (var point in points)
                {
                    if (Graphics.CircleContainsPoint(filament, point))
                    {
                        intersects_filament = true;
                        var ray = new Line();
                        ray.X1 = LEDB.X;
                        ray.Y1 = LEDB.Y;
                        ray.X2 = point.X;
                        ray.Y2 = point.Y;
                        // Set Line's width and color
                        SolidColorBrush redBrush = new SolidColorBrush();
                        redBrush.Color = Colors.Red;
                        redBrush.Opacity = 0.1;
                        ray.StrokeThickness = width * 2;
                        ray.Stroke = redBrush;
                        LayoutRoot.Children.Add(ray);
                        break;
                    }
                }

                // Falling edge detection
                if (!previousIntersection && intersects_filament)
                {
                    falling_edge = t;
                    EdgeBDetected_0 = new Point(falling_edge, EdgeB_0.Y);
                }
                // Rising edge detection
                if (!intersects_filament && previousIntersection)
                {
                    rising_edge = t - 1;
                    EdgeBDetected_1 = new Point(rising_edge, EdgeB_0.Y);
                }

                if (!intersects_filament)
                {
                    SolidColorBrush redBrush = new SolidColorBrush();
                    redBrush.Color = Colors.Red;
                    redBrush.Opacity = 0.1;
                    var ray = new Line();
                    ray.X1 = LEDB.X;
                    ray.Y1 = LEDB.Y;
                    ray.X2 = t;
                    ray.Y2 = EdgeB_0.Y;
                    // Set Line's width and color
                    ray.StrokeThickness = width * 2;
                    ray.Stroke = redBrush;
                    LayoutRoot.Children.Add(ray);
                }
                previousIntersection = intersects_filament;

                if (!Double.IsNaN(rising_edge) && !Double.IsNaN(falling_edge))
                {
                    widthB = (rising_edge - falling_edge);
                }
            }
        }

    }
}

// if falling edge, set edgeA_0 = pos
// if rising edge, set edgeA_1 = pos