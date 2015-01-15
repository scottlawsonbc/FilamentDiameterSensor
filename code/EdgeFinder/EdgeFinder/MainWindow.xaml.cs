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
        const double DIST_MULT = 10.0;

        BrushConverter converter = new System.Windows.Media.BrushConverter();
        Brush ledColor, validColor, errorColor;

        // Variables for drag and drop detection
        bool captured = false;
        double x_shape, x_canvas, y_shape, y_canvas;
        UIElement source = null;
        Line l0s0, l1s0, lms0;
        Line l0s1, l1s1, lms1;

        // Coordinates for the shapes
        double led0X, led0Y, led1X, led1Y;
        double sensor0X, sensor0Y, sensor1X, sensor1Y;
        double filamentX, filamentY;

        bool deltaXAllowed = true;
        bool deltaYAllowed = true;

        private void InitBrushes()
        {
            errorColor = (Brush)converter.ConvertFromString("#8d4b84");
            validColor = (Brush)converter.ConvertFromString("#4b8d54");
            ledColor = (Brush)converter.ConvertFromString("#c71153");
        }

        private void UpdateCoords()
        {
            led0X = Canvas.GetLeft(led0) + led0.Width / 2;
            led0Y = Canvas.GetTop(led0) + led0.Height / 2;
            led1X = Canvas.GetLeft(led1) + led1.Width / 2;
            led1Y = Canvas.GetTop(led1) + led1.Height / 2;
            filamentX = Canvas.GetLeft(filament) + filament.Width / 2;
            filamentY = Canvas.GetTop(filament) + filament.Height / 2;
            sensor0X = Canvas.GetLeft(sensor0) + sensor0.Width / 2;
            sensor0Y = Canvas.GetTop(sensor0) + sensor0.Height / 2;
            sensor1X = Canvas.GetLeft(sensor1) + sensor1.Width / 2;
            sensor1Y = Canvas.GetTop(sensor1) + sensor1.Height / 2;
        }

        public MainWindow()
        {
            InitializeComponent();
            InitBrushes();
            InitLines();
            UpdateCoords();
            DetectionPossible();
            UpdateDistances();
            RunUserCode();
        }

        private void UpdateDistances()
        {
            UpdateCoords();
            // Distances 0
            double dist0x0 = (filamentX - led0X) / DIST_MULT;
            double dist0x1 = ((sensor0X - sensor0.Width / 2) - filamentX) / DIST_MULT;
            double dist0Total = dist0x0 + dist0x1; // Math.Sqrt(dist0x0 * dist0x0 + dist0x1 * dist0x1);
            var dist0Text = string.Format("{0:F2} mm, {1:F2} mm", dist0x0, dist0x1);
            var dist0TotalText = string.Format("{0:F2} mm", dist0Total);
            distances0Text.Text = dist0Text;
            totalDistance0Text.Text = dist0TotalText;

            // Distances 1
            double dist1x0 = (led1Y - filamentY) / DIST_MULT;
            double dist1x1 = (filamentY - (sensor1Y + sensor1.Width / 2)) / DIST_MULT;
            double dist1Total = dist1x0 + dist1x1; //Math.Sqrt(dist1x0 * dist1x0 + dist1x1 * dist1x1);
            var dist1Text = string.Format("{0:F2} mm, {1:F2} mm", dist1x0, dist1x1);
            var dist1TotalText = string.Format("{0:F2} mm", dist1Total);
            distances1Text.Text = dist1Text;
            totalDistance1Text.Text = dist1TotalText;

            // Edges 0
            bool sensor0Hit = (l0s0.Y2 > (sensor0Y - sensor0.Height / 2)) && (l1s0.Y2 < (sensor0Y + sensor0.Height / 2)) && (sensor0X - sensor0.Width / 2 > filamentX + filament.Width / 2);
            if (sensor0Hit)
            {
                double sensor0edge0 = sensor0Y + sensor0.Height / 2.0 - l0s0.Y2;
                double sensor0edge1 = sensor0Y + sensor0.Height / 2.0 - l1s0.Y2;
                double width = Math.Abs(sensor0edge1 - sensor0edge0) / DIST_MULT;
                var edgeText = string.Format("({0:F2}, {1:F2})", sensor0edge0, sensor0edge1);
                var widthText = string.Format("{0:F3} mm", width);
                edges0Text.Text = edgeText;
                width0Text.Text = widthText;
            }
            else
            {
                edges0Text.Text = "--";
                width0Text.Text = "--";
            }

            // Edges 1
            bool sensor1Hit = (l0s1.X2 > (sensor1X - sensor1.Width / 2)) && (l1s1.X2 < (sensor1X + sensor1.Width / 2)) && (sensor1Y + sensor1.Height / 2 < filamentY - filament.Height / 2);
            if (sensor1Hit)
            {
                double sensor1edge0 = l0s1.X2 - sensor1X + sensor1.Width / 2.0;
                double sensor1edge1 = l1s1.X2 - sensor1X + sensor1.Width / 2.0;
                double width = Math.Abs(sensor1edge1 - sensor1edge0) / DIST_MULT;
                var edgeText = string.Format("({0:F2}, {1:F2})", sensor1edge0, sensor1edge1);
                var widthText = string.Format("{0:F3} mm", width);
                edges1Text.Text = edgeText;
                width1Text.Text = widthText;
            }
            else
            {
                edges1Text.Text = "--";
                width1Text.Text = "--";
            }

            // X Offset
            double xOffset = (Canvas.GetLeft(sensor1) - (Canvas.GetLeft(sensor0) - sensor1.Width)) / DIST_MULT;
            xOffsetText.Text = string.Format("{0:F2} mm", xOffset);

            // Y Offset
            double yOffset = (Canvas.GetTop(sensor1) - (Canvas.GetTop(sensor0) - sensor1.Height)) / DIST_MULT;
            yOffsetText.Text = string.Format("{0:F2} mm", yOffset);

        }

        private void InitLines()
        {
            UpdateCoords();
            // Shadow edge lines
            l0s0 = new Line();
            l1s0 = new Line();
            l0s1 = new Line();
            l1s1 = new Line();
            // Midpoint lines
            lms0 = new Line();
            lms1 = new Line();

            // Stroke
            l0s0.Stroke = ledColor;
            l1s0.Stroke = ledColor;
            l0s1.Stroke = ledColor;
            l1s1.Stroke = ledColor;
            lms0.Stroke = Brushes.Gray;
            lms1.Stroke = Brushes.Gray;

            lms0.StrokeDashArray = new DoubleCollection(new double[] { 20.0, 20.0 });
            lms1.StrokeDashArray = new DoubleCollection(new double[] { 20.0, 20.0 });

            // Stroke width
            l0s0.StrokeThickness = 0.75;
            l1s0.StrokeThickness = .75;
            l0s1.StrokeThickness = .75;
            l1s1.StrokeThickness = .75;
            lms0.StrokeThickness = .75;
            lms1.StrokeThickness = .75;

            // Add to main canvas
            DrawLines();
            LayoutRoot.Children.Add(l0s0);
            LayoutRoot.Children.Add(l1s0);
            LayoutRoot.Children.Add(lms0);
            LayoutRoot.Children.Add(l0s1);
            LayoutRoot.Children.Add(l1s1);
            LayoutRoot.Children.Add(lms1);
        }

        private void DrawLines()
        {
            UpdateCoords();
            // X1 Coords
            l0s0.X1 = led0X;
            l1s0.X1 = led0X;
            lms0.X1 = led0X;
            l0s1.X1 = led1X;
            l1s1.X1 = led1X;
            lms1.X1 = led1X;

            // Y1 coords
            l0s0.Y1 = led0Y;
            l1s0.Y1 = led0Y;
            lms0.Y1 = led0Y;
            l0s1.Y1 = led1Y;
            l1s1.Y1 = led1Y;
            lms1.Y1 = led1Y;

            // X2 Coords
            l0s0.X2 = filamentX;
            l1s0.X2 = filamentX;
            lms0.X2 = filamentX;
            l0s1.X2 = filamentX - filament.Width / 2;
            l1s1.X2 = filamentX + filament.Width / 2;
            lms1.X2 = filamentX;

            // Y2 coords
            l0s0.Y2 = filamentY - filament.Height / 2;
            l1s0.Y2 = filamentY + filament.Height / 2;
            lms0.Y2 = filamentY;
            l0s1.Y2 = filamentY;
            l1s1.Y2 = filamentY;
            lms1.Y2 = filamentY;

            // Extend the lines to the sensors
            double dist0 = Math.Sqrt((sensor0X - filamentX) * (sensor0X - filamentX) + (sensor0Y - filamentY) * (sensor0Y - filamentY));
            double dist1 = Math.Sqrt((sensor1Y - filamentY) * (sensor1Y - filamentY) + (sensor1X - filamentX) * (sensor1X - filamentX));
            double len0 = Math.Sqrt((l0s0.X2 - l0s0.X1) * (l0s0.X2 - l0s0.X1) + (l0s0.Y2 - l0s0.Y1) * (l0s0.Y2 - l0s0.Y1));
            double len1 = Math.Sqrt((l0s1.X2 - l0s1.X1) * (l0s1.X2 - l0s1.X1) + (l0s1.Y2 - l0s1.Y1) * (l0s1.Y2 - l0s1.Y1));

            // Sensor 0
            l0s0.X2 = l0s0.X2 + (l0s0.X2 - l0s0.X1) / len0 * dist0;
            l0s0.Y2 = l0s0.Y2 + (l0s0.Y2 - l0s0.Y1) / len0 * dist0;
            l1s0.X2 = l0s0.X2;
            l1s0.Y2 = l1s0.Y2 + (l1s0.Y2 - l1s0.Y1) / len0 * dist0;
            lms0.X2 = l0s0.X2;
            lms0.Y2 = lms0.Y2 + (lms0.Y2 - lms0.Y1) / len0 * dist0;

            // Sensor 1
            l0s1.X2 = l0s1.X2 + (l0s1.X2 - l0s1.X1) / len1 * dist1;
            l1s1.X2 = l1s1.X2 + (l1s1.X2 - l1s1.X1) / len1 * dist1;
            l0s1.Y2 = l0s1.Y2 + (l0s1.Y2 - l0s1.Y1) / len1 * dist1;
            l1s1.Y2 = l1s1.Y2 + (l1s1.Y2 - l1s1.Y1) / len1 * dist1;
            lms1.X2 = lms1.X2 + (lms1.X2 - lms1.X1) / len1 * dist1;
            lms1.Y2 = lms1.Y2 + (lms1.Y2 - lms1.Y1) / len1 * dist1;
        }

        private bool DetectionPossible()
        {
            UpdateCoords();
            bool sensor0Hit = (l0s0.Y2 > (sensor0Y - sensor0.Height / 2)) && (l1s0.Y2 < (sensor0Y + sensor0.Height / 2)) && (sensor0X - sensor0.Width / 2 > filamentX + filament.Width / 2);
            bool sensor1Hit = (l0s1.X2 > (sensor1X - sensor1.Width / 2)) && (l1s1.X2 < (sensor1X + sensor1.Width / 2)) && (sensor1Y + sensor1.Height / 2 < filamentY - filament.Height / 2);

            if (sensor0Hit)
            {
                sensor0.Fill = validColor;
            }
            else
            {
                sensor0.Fill = errorColor;
            }
            if (sensor1Hit)
            {
                sensor1.Fill = validColor;
            }
            else
            {
                sensor1.Fill = errorColor;
            }

            return true;
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
                UpdateCoords();
                DrawLines();
                DetectionPossible();
                UpdateDistances();
                RunUserCode();
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

        double xWidth, yWidth, xDist, yDist, xOffset, yOffset;

        private void setValues()
        {
            double sensor0edge0 = (sensor0Y + sensor0.Height / 2.0 - l0s0.Y2) / DIST_MULT;
            double sensor0edge1 = (sensor0Y + sensor0.Height / 2.0 - l1s0.Y2) / DIST_MULT;

            double sensor1edge0 = (l0s1.X2 - sensor1X + sensor1.Width / 2.0) / DIST_MULT;
            double sensor1edge1 = (l1s1.X2 - sensor1X + sensor1.Width / 2.0) / DIST_MULT;

            xWidth = Math.Abs((sensor0Y + sensor0.Height / 2.0 - l0s0.Y2) / DIST_MULT - (sensor0Y + sensor0.Height / 2.0 - l1s0.Y2) / DIST_MULT);
            yWidth = Math.Abs((l0s1.X2 - sensor1X - sensor1.Width / 2.0) / DIST_MULT - (l1s1.X2 - sensor1X - sensor1.Width / 2.0) / DIST_MULT);

            xOffset = (Canvas.GetLeft(sensor0) - (Canvas.GetLeft(sensor1) + sensor1.Width)) / DIST_MULT;
            yOffset = -(Canvas.GetTop(sensor1) + sensor1.Height - Canvas.GetTop(sensor0)) / DIST_MULT;

            double filx = Canvas.GetLeft(sensor1) + sensor1.Width;
            filx -= l1s1.X2 - yWidth / 2.0;
            filx /= DIST_MULT;
            xOffset += filx;

            double fily = Canvas.GetTop(sensor0) + sensor0.Height;
            fily -= l1s0.Y2 - xWidth / 2.0;
            fily /= DIST_MULT;
            yOffset -= fily;


            double dist0x0 = (filamentX - led0X) / DIST_MULT;
            double dist0x1 = ((sensor0X - sensor0.Width / 2) - filamentX) / DIST_MULT;
            xDist = Math.Sqrt(dist0x0 * dist0x0 + dist0x1 * dist0x1);

            double dist1x0 = (led1Y - filamentY) / DIST_MULT;
            double dist1x1 = (filamentY - (sensor1Y + sensor1.Width / 2)) / DIST_MULT;
            yDist = Math.Sqrt(dist1x0 * dist1x0 + dist1x1 * dist1x1);
        }

        SensorCoordinates A;
        SensorCoordinates B;
        public void SetCoordinates()
        {
            double origin_x = Canvas.GetLeft(sensor0) + sensor0.Width;
            double origin_y = Canvas.GetTop(sensor1);
            
            // A coordinates
            A.LED_x = (origin_x-(Canvas.GetLeft(led0) + led0.Width / 2.0))/DIST_MULT;
            A.LED_y = ((Canvas.GetTop(led0) + led0.Height / 2.0) - origin_y) / DIST_MULT;
            A.edge_0 = (l0s0.Y2 - origin_y) / DIST_MULT;
            A.edge_1 = (l1s0.Y2 - origin_y) / DIST_MULT;

            // B coordinates
            B.LED_x = (origin_x - (Canvas.GetLeft(led1) + led1.Width / 2.0)) / DIST_MULT;
            B.LED_y = ((Canvas.GetTop(led1) + led1.Height / 2.0) - origin_y) / DIST_MULT;
            B.edge_0 = (origin_x - l1s1.X2) / DIST_MULT;
            B.edge_1 = (origin_x - l0s1.X2) / DIST_MULT;
        }
    }
}
