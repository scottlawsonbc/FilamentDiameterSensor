namespace Extensions
{
    using System;
    using System.Windows;
    using System.Windows.Controls;
    using System.Windows.Data;
    using System.Windows.Documents;
    using System.Windows.Input;
    using System.Windows.Media;
    using System.Windows.Media.Imaging;
    using System.Windows.Navigation;
    using System.Windows.Shapes;

    public static class ExtensionMethods
    {
        // Returns the radius of the circle
        public static double Radius(this Ellipse circle)
        {
            if (circle.Width != circle.Height) throw new ArgumentException("Argument does not represent a circle");
            return circle.Width / 2.0;
        }

        // Returns the radius of the circle
        public static double Radius(this UIElement circle)
        {
            var h = circle.DesiredSize.Height;
            var w = circle.DesiredSize.Width;

            if (w != h) throw new ArgumentException("Argument does not represent a circle");
            return w / 2.0;
        }

        // Returns the slope of the line
        public static double Slope(this Line l)
        {
            var dx = l.X2 - l.X1;
            var dy = l.Y2 - l.Y1;
            return dy / dx;
        }

        //public static List<Point> 

    }
}