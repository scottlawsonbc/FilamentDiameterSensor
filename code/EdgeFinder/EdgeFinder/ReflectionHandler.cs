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
    public partial class MainWindow
    {
        const string CODE_PATH = "calculator-code.cs";
        const string DEFAULT_CODE = @"using System;
namespace X
{
    public class X
    {
        public double ComputeDiameter(double xWidth, double yWidth, double xOffset, double yOffset, double xDist, double yDist)
        {
            return 0.0;
        }
    }
}";
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
            SetCoordinates();
            var lastModified = System.IO.File.GetLastWriteTime(CODE_PATH);

            // Check if the code has been modified since the last compilation
            if (DateTime.Compare(lastModified, lastModifiedTime) > 0)
            {
                CompileUserCode();
                lastModifiedTime = lastModified;
            }
            if (compilationSuccessful)
            {
                filamentDiameterText.Text = string.Format("{0:F3} mm", ExecuteUserCode());
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
                return (double)mi.Invoke(o, new object[] { A.LED_x, A.LED_y,
                                                           B.LED_x, B.LED_y,
                                                           A.edge_0, A.edge_1,
                                                           B.edge_0, B.edge_1});
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message, "Code compiled, but could not executed");
                return 0.0;
            }
        }
    }
}
