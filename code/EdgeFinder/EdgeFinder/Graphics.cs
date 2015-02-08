using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
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

    public class Graphics
    {

        public static bool CircleContainsPoint(UIElement circle, Point p)
        {
            
            var r = circle.Radius();

            var x = Canvas.GetLeft(circle) + circle.DesiredSize.Width/2.0;
            var y = Canvas.GetTop(circle) + circle.DesiredSize.Height / 2.0;

            var dx = (x - p.X);
            var dy = (y - p.Y);
            var dist = ((dx * dx) + (dy * dy));
            var rsquared = r * r;
            return dist <= rsquared;
        }

        
        

        //// Returns the points of intersection between a line and circle
        //public static Nullable<Point[]> Intersection(Line line, Ellipse circle)
        //{
        //    var slope = line.Slope();
        //    var center = circle.Center();

        //    var points = new List<Point>();
        //    for (int i = (int)Canvas.GetLeft(circle); i <= (int)Canvas.GetRight(circle); i++)
        //    {
        //        var p = new Point((float)i, (float)(i - line.X1) * slope + line.Y1);
        //    }



        //    //points = from p in points 
        //    //         where p.x



        //    return null;
        //}
    }
}
