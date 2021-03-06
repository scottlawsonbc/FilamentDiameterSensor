﻿using System.Net;
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
    public partial class MainWindow
    {
        const string CODE_PATH = "calculator-code.cs";
        const string DEFAULT_CODE = @"using System;

namespace X
{

public class DiameterCalculator
{
    public double ComputeDiameter(double LEDA_x, double LEDA_y,
                                  double LEDB_x, double LEDB_y,
                                  double edgeA_0, double edgeA_1,
                                  double edgeB_0, double edgeB_1)
    {
        // Location of the center of filament
        double filament_x = (edgeB_0 + edgeB_1) / 2.0;
        double filament_y = (edgeA_0 + edgeA_1) / 2.0;

        // Diameter for A
        double LEDA_to_filament = LEDA_x - filament_x;
        double A_diameter = (LEDA_to_filament * (edgeA_1 - edgeA_0)) / LEDA_x;

        // Diameter for B fc
        double LEDB_to_filament = filament_y - LEDB_y;
        double B_diameter = (LEDB_to_filament * (edgeB_1 - edgeB_0)) / LEDB_y;

        // Return the mean of the two diameters
        return (A_diameter + B_diameter) / 20.0;
    }
}
}

";
        // Contains the results of the realtime compilation
        object o;
        DateTime lastModifiedTime = DateTime.Now.AddYears(-10); // Tracks the last time the code file was modified
        bool compilationSuccessful = false;       // Indicates whether the previous compilation was successful
        string lastErrorMessage = "";             // Last known compiler error message

        private void CheckCodeExists()
        {
            if (!System.IO.File.Exists(CODE_PATH))
            {
                System.IO.File.Create(CODE_PATH);
                System.IO.File.WriteAllText(CODE_PATH, DEFAULT_CODE);
            }
        }

        // Event handler for openCodeButton
        private void ShowCodeButton_Click(object sender, EventArgs e)
        {
            CheckCodeExists();
            Process.Start(CODE_PATH);
        }

        // Event handler for lastErrorButton
        private void ShowLastError_Click(object sender, EventArgs e)
        {
            // Make the label that contains the error message
            var errorText = new System.Windows.Forms.TextBox();
            errorText.AcceptsReturn = true;
            errorText.AcceptsTab = true;
            errorText.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            errorText.Dock = System.Windows.Forms.DockStyle.Fill;
            errorText.Multiline = true;
            errorText.Text = lastErrorMessage;

            // Make the form that will show the error message
            var errorForm = new System.Windows.Forms.Form();
            errorForm.Text = "Compilation errors";
            errorForm.Height = 600;
            errorForm.Width = 800;
            errorForm.Controls.Add(errorText);
            errorForm.Show();
        }

        private void RunUserCode()
        {
            CheckCodeExists();
            var lastModified = System.IO.File.GetLastWriteTime(CODE_PATH);

            // Check if the code has been modified since the last compilation
            if (DateTime.Compare(lastModified, lastModifiedTime) > 0)
            {
                CompileUserCode();
                lastModifiedTime = lastModified;
            }
            if (compilationSuccessful)
            {
                UpdateMeasurements(ExecuteUserCode());
            }
        }

        private void UpdateMeasurements(double diameter)
        {
            double diameterError = 100.0 - diameter / 3.00 * 100.00;
            double areaError = 100.0 - (diameter * diameter) / 9.00 * 100.0;

            filamentDiameterText.Text = string.Format("{0:F3} mm", diameter);

            if (diameterError > 0.0)
            {
                filamentErrorText.Text = string.Format("+{0:F2}%", diameterError);
                areaErrorText.Text = string.Format("+{0:F2}%", areaError);
            }
            else
            {
                filamentErrorText.Text = string.Format("{0:F2}%", diameterError);
                areaErrorText.Text = string.Format("{0:F2}%", areaError);
            }
        }

        private void CompileUserCode()
        {
            var provider = new CSharpCodeProvider(new Dictionary<string, string> { { "CompilerVersion", "v3.5" } });
            var compilerParams = new CompilerParameters
            {
                GenerateInMemory = true,
                GenerateExecutable = false
            };

            try
            {
                var compilationResult = provider.CompileAssemblyFromFile(compilerParams, CODE_PATH);

                // Check for compilation errors
                if (compilationResult.Errors.Count != 0)
                {
                    string message = "";
                    foreach (var s in compilationResult.Output)
                    {
                        message += s + Environment.NewLine;
                    }

                    lastErrorMessage = message;
                    compilationSuccessful = false;
                    lastErrorButton.Background = Brushes.LightPink;
                }
                else
                {
                    lastErrorButton.Background = Brushes.White;
                    compilationSuccessful = true;
                    lastErrorMessage = "Compilation successful";
                    o = compilationResult.CompiledAssembly.CreateInstance("X.DiameterCalculator");
                }
            }
            catch (Exception e)
            {
                compilationSuccessful = false;
                lastErrorMessage = e.Message;
            }
        }

        // Execute the user's code 
        private double ExecuteUserCode()
        {
            try
            {
                MethodInfo mi = o.GetType().GetMethod("ComputeDiameter");
                return (double)mi.Invoke(o, new object[] { EdgeA_0.X - LEDA.X, LEDA.Y - EdgeB_0.Y,
                                                           EdgeA_0.X - LEDB.X, LEDB.Y - EdgeB_0.Y,                                
                                                           EdgeADetected_0.Y - EdgeB_0.Y, EdgeADetected_1.Y - EdgeB_0.Y,
                                                           EdgeA_0.X - EdgeBDetected_0.X, EdgeA_0.X - EdgeBDetected_1.X});
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message, "Code compiled, but could not be executed");
                return 0.0;
            }
            return 2.0;
        }
    }
}
