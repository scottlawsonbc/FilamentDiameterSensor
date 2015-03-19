namespace ShadowSim

module Graphics =
    open System
    open System.Drawing
    open System.Drawing.Drawing2D
    open System.Windows.Forms
    
    let Width  = 600
    let Height = 600

    // LEDs
    let LED_radius = 10
    let LEDA_start = new Point(Width * 1/5, Height * 1/5-LED_radius)
    let LEDB_start = new Point(Width * 4/5-LED_radius, Height * 4/5)
    let LEDA = ref(new Rectangle(LEDA_start.X, LEDA_start.Y, LED_radius*2, LED_radius*2))
    let LEDB = ref(new Rectangle(LEDB_start.X, LEDB_start.Y, LED_radius*2, LED_radius*2))
    
    // Sensors
    let Sensor_length    = int 100
    let Sensor_thickness = 10
    let SensorA_start = new Point(Width-Sensor_thickness-15, Height * 1/10)
    let SensorB_start = new Point(Width * 7/10, Height * 0/10)
    let SensorA = ref(new Rectangle(SensorA_start.X, SensorA_start.Y, Sensor_thickness, Sensor_length))
    let SensorB = ref(new Rectangle(SensorB_start.X, SensorB_start.Y, Sensor_length, Sensor_thickness))

    // Filament
    let Filament_diameter = 30
    let Filament_color = Color.Green
    let Filament_start = new Point(Width * 8/10 - Filament_diameter, Height * 1/5-Filament_diameter)
    let Filament = ref(new Rectangle(Filament_start.X, Filament_start.Y, Filament_diameter, Filament_diameter))

    let Shapes = [|LEDA; LEDB; SensorA; SensorB; Filament|]


    let points (p0:Point) (p1:Point) =
        let x0 = p0.X
        let y0 = p0.Y
        let x1 = p1.X
        let y1 = p1.Y
        let slope = float(y1-y0)/float(x1-x0)
        List.map(fun (x:int) -> new Point(x, y0 + int(slope*float(x-x0)))) [x0 .. x1]

    let raytrace(b:Bitmap, g:Graphics) = 
        // LEDA -> SensorA
        let x0 = (!LEDA).X + LED_radius
        //let y0 = (!LEDA).Y + LED_radius
        for y0 = (!LEDA).Y + LED_radius to (!LEDA).Y + LED_radius + 1 do
            let startPoint = new Point(x0,y0)
            for y = (!SensorA).Top to (!SensorA).Bottom do
                let endPoint = new Point((!SensorA).Left, y)
                let intersectionPoint = List.tryFind (fun (point:Point) -> b.GetPixel(point.X, point.Y).G > byte(1)) (points startPoint endPoint)
                match intersectionPoint with
                | Some(x) -> g.DrawLine(new Pen(Color.Red), startPoint, x)
                | None ->    g.DrawLine(new Pen(Color.Red), startPoint, endPoint)
     









//    let intersects (rA:Rectangle, dataA:Color[]) (rB:Rectangle, dataB:Color[]) = 
//        let top    = Math.Max(rA.Top,    rB.Top)
//        let bottom = Math.Min(rA.Bottom, rB.Bottom)
//        let left   = Math.Max(rA.Left,   rB.Left)
//        let right  = Math.Min(rA.Right,  rB.Right)
//
//        let intersection = ref false
//
//        for y = top to bottom do
//            for x = left to right do
//                // Get the color of both pixels at this point
//                let colorA = dataA.[(x-rA.Left)+(y-rA.Top)*rA.Width]
//                let colorB = dataB.[(x-rB.Left)+(y-rB.Top)*rB.Width]
//                // Intersection if both pixels are not transparent
//                if colorA.A <> byte(0) && colorB.A <> byte(0) then
//                    intersection := true
//        intersection.Value