namespace ShadowSim

module Sim = 
    open System
    open System.Drawing
    open System.Drawing.Drawing2D
    open System.Windows.Forms
    open System.Threading.Tasks
    

    let f = new Form()
    let img = new PictureBox(Dock = DockStyle.Fill)
    f.Width <- 600
    f.Height <- 600
    f.FormBorderStyle <- FormBorderStyle.FixedToolWindow
    f.Text <- "ShadowSim"
    f.BackColor <- Color.Black//ColorTranslator.FromHtml("#242424")
    f.Controls.Add(img)
    
    let bitmap = new Bitmap(f.Width, f.Height)
    let g = Graphics.FromImage(bitmap)
   
    let draw() = 
        g.Clear(Color.Black)//ColorTranslator.FromHtml("#242424"))
        // Draw LEDs
        g.FillEllipse(Brushes.Red, Graphics.LEDA.contents)
        g.FillEllipse(Brushes.Red, Graphics.LEDB.contents)
        // Draw Sensors
        g.FillRectangle(Brushes.Gray, Graphics.SensorA.contents)
        g.FillRectangle(Brushes.Gray, Graphics.SensorB.contents)
        // Draw Filament 
        g.FillEllipse(new SolidBrush(Graphics.Filament_color), Graphics.Filament.contents)
        if img.Image <> null then
            Graphics.raytrace(img.Image :?> Bitmap, g);

        img.Refresh()
        f.Refresh()
        
        

    
    // Used for tracking drap and drop state
    let mutable captured = false
    let mutable capturedShape = ref (new Rectangle())
    let mutable lastX, lastY = 0, 0

    let mouseDown (e:EventArgs) = 
        let args = e :?> MouseEventArgs
        if args.Button = MouseButtons.Left then
            match Array.tryFind(fun (o:Rectangle ref) -> o.contents.Contains(args.X, args.Y)) Graphics.Shapes with
            | Some(o) -> capturedShape <- o; captured <- true; lastX <- args.X; lastY <- args.Y
            | None -> ()
                   
    let mouseUp (e:EventArgs) = 
        let args = e :?> MouseEventArgs
        if args.Button = MouseButtons.Left then captured <- false

    let mouseMove (e:EventArgs) =
        if captured then
            let args = e :?> MouseEventArgs
            let dx = args.X - lastX
            let dy = args.Y - lastY
            
            let length = Graphics.Sensor_length
            let thickness = Graphics.Sensor_thickness
            
            if capturedShape.contents.Width = Graphics.Sensor_length then
                capturedShape.contents.X <- (!capturedShape).X + dx
            else if capturedShape.contents.Width = Graphics.Sensor_thickness then
                capturedShape.contents.Y <- (!capturedShape).Y + dy 
            else
                capturedShape.contents.X <- (!capturedShape).X + dx
                capturedShape.contents.Y <- (!capturedShape).Y + dy
            draw()
            lastX <- args.X
            lastY <- args.Y
       
    draw()
    img.MouseDown.Add mouseDown
    img.MouseUp.Add mouseUp
    img.MouseMove.Add mouseMove
    img.Image <- bitmap
    Application.Run(f)   








































//    let points (p0:Point) (p1:Point) =
//        let x0 = p0.X
//        let y0 = p0.Y
//        let x1 = p1.X
//        let y1 = p1.Y
//        let slope = float(y1-y0)/float(x1-x0)
//        List.map(fun (x:int) -> new Point(x, y0 + int(slope*float(x-x0)))) [x0 .. x1]
//
//    let raycast(g:Graphics) = 
//        for i = sensor.Top to sensor.Bottom do
//            let startPoint = new Point(lightSource.Left + lightSourceRadius, lightSource.Top + lightSourceRadius)
//            let endPoint = new Point(sensor.Left, i)
//            
//            let intersectionPoint = List.tryFind (fun (point:Point) -> filamentPath.IsVisible point) (points startPoint endPoint)
//            if intersectionPoint.IsSome then
//                g.DrawLine(new Pen(Color.PaleVioletRed), startPoint, intersectionPoint.Value)
//            else
//                g.DrawLine(new Pen(Color.PaleVioletRed), startPoint, endPoint)

//    // Called when screen should be redrawn
//    let paint(e:PaintEventArgs) = 
//        let g = f.CreateGraphics()
//        drawSensor g
//        drawFilament g
//        drawLightSource g
//        raycast g
//        ()
//
//    f.Click.Add(fun _ -> 
//        sensor.Location <- new Point(sensor.Left - 10, sensor.Top)
//        let x = filamentPath.PathPoints
//        f.Refresh())

    //f.Paint.Add paint 