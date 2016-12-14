using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Emgu.CV;
using Emgu.CV.Util;
using Emgu.CV.Structure;
using Microsoft.Xna.Framework;
using System.Diagnostics;
using System.Net.Sockets;
using System.IO;

namespace KTLib
{
    public class TrackerManager
    {
        KinectInterface kinect;
        public BallDetector detector;
        BallTracker tracker;
        int counter = 0;
        static TcpClient tcpClient = new TcpClient();
        Stream stream;

        public Image<Bgr, byte> DisplayOut;

        public bool Ready = false;

        public BallTrackData ActiveBall;
        //Stopwatch stopwatch = new Stopwatch();

        public TrackerManager(KinectInterface kinect)
        {
            this.kinect = kinect;
            kinect.OnDepthFrame += new Action(kinect_OnDepthFrame);
            //tcpClient.Connect("192.168.0.100", 9999);
            //stream = tcpClient.GetStream();
            //stopwatch.Start();

            detector = new BallDetector();
            tracker = new BallTracker();
        }

        void kinect_OnDepthFrame()
        {
            DateTime time = DateTime.Now;

            if (!Ready)
            {
                Console.WriteLine("");
            }
            var balls = detector.Detect(kinect);
            foreach (var ball in balls)
            {
                Frame3D frame = new Frame3D() { Ball = ball, Time = time };
                tracker.Push(frame);
            }

            Ready = false;
            counter++;

            /*if (counter > 24)
            {
                stream.Write(Encoding.UTF8.GetBytes("test".ToCharArray()), 0, "test".Length);
                counter = 0;
            }*/

            DisplayOut = detector.DetectorOverlay.Copy();
            drawPrediction();

            Ready = true;
        }

        public void Update()
        {
            tracker.Update();

            ActiveBall = tracker.GetMostActiveBallProj();
        }

        void drawPrediction()
        {
            if (ActiveBall != null)
            {
                List<System.Drawing.Point> line = new List<System.Drawing.Point>();
                double predictSec = 2;
                for (double t = 0; t <= predictSec; t += 0.5 /*0.01*/)
                {

                    var tsamp = DateTime.Now.AddSeconds(t);
                    Vector2 unproj;

                    if (kinect.ProjectToPx(ActiveBall.ProjFit.PredictPos(tsamp).ToV3(), out unproj)
                        /*&& unproj.X >= 0 && unproj.X < KinectInterface.w*/
                        && unproj.Y >= 0 && unproj.Y < KinectInterface.h
                        )
                    {
                        //Console.WriteLine("Time elapsed: {0}", stopwatch.Elapsed);
                        unproj *= 0.5f;
                        Vector3 v3 = ActiveBall.ProjFit.PredictPos(tsamp).ToV3();
                        string data = "X: " + v3.X.ToString("0.00") + " Y: " + v3.Y.ToString("0.00") + " Z: " + (v3.Z-0.5).ToString("0.00");
                        Debug.WriteLine(data);
                        //string data2 = "Unproj: X: " + unproj.X.ToString("0.00") + " Y: " + unproj.Y.ToString("0.00");
                        //118.5
                        double as1 = (Math.Sin(2.06821516));
                        double as2 = as1 * Math.Abs(v3.X);
                        double as3 = (v3.Z - 0.5);
                        double as4 = as2 / as3;
                        double A = Math.Asin(as4);
                        A = A * 180 / Math.PI;
                        //double A = Math.Asin((as2) / (v3.Z - 0.5));
                        //Debug.WriteLine(A);

                        double B = 180 - A - 118.5;
                        Debug.WriteLine(B);

                        double b = ((Math.Sin(B * Math.PI / 180)) * Math.Abs(v3.X)) / (Math.Sin(A * Math.PI / 180));


                        double A2 = 90 - B;

                        double XSquare = ((Math.Sin(A2 * Math.PI / 180)) * (v3.Z - 0.5)) / (Math.Sin(90 * Math.PI / 180));
                        Debug.WriteLine("X trekant: " + XSquare);

                        double YSquare = ((Math.Sin(B * Math.PI / 180)) * (v3.Z - 0.5)) / (Math.Sin(90 * Math.PI / 180));
                        Debug.WriteLine("Y trekant: " + YSquare);

                        double finalx = (1 - XSquare) * 1000;
                        double finaly = (1.2 - YSquare) * 1000;

                        //Debug.WriteLine(data2);
                        tcpClient = new TcpClient();
                        tcpClient.Connect("192.168.0.100", 9999);
                        stream = tcpClient.GetStream();
                        //double pos = Math.Pow(v3.Z-0.5, 2) - 1;
                        //pos = Math.Sqrt(pos);
                        //pos = pos - 0.94;
                        //pos = pos * 1000;
                        string data3 = (finalx.ToString() + ";" + finaly.ToString());
                        Debug.WriteLine(data3);

                        //stream.Write(Encoding.UTF8.GetBytes((pos.ToString() + ";" + "300").ToCharArray()), 0, (pos.ToString() + ";" + "300").Length);
                        stream.Write(Encoding.UTF8.GetBytes((finalx.ToString() + ";" + finaly.ToString()).ToCharArray()), 0, (finalx.ToString() + ";" + finaly.ToString()).Length);
                        /*stream.Write(Encoding.UTF8.GetBytes(("Y: " + v3.Y).ToCharArray()), 0, ("Y: " + v3.Y).Length);
                        stream.Write(Encoding.UTF8.GetBytes(("Z: " + v3.Z).ToCharArray()), 0, ("Z: " + v3.Z).Length);
                        stream.Write(Encoding.UTF8.GetBytes(data.ToCharArray()), 0, data.Length);*/
                        tcpClient.Close();
                        //Console.WriteLine("Time elapsed: {0}", stopwatch.Elapsed);
                        
                        System.Drawing.PointF pt = new System.Drawing.PointF(unproj.X, unproj.Y);
                        //depthMaskOverlay.Draw(new Cross2DF(pt, 10, 10), new Bgr(0, 0, 255), 3);
                        line.Add(new System.Drawing.Point((int)unproj.X, (int)unproj.Y));
                        //line.Add(new System.Drawing.Point((int)(t*100), (int)(t*100) - 100));
                    }
                    
                }
                Debug.WriteLine("");
                DisplayOut.DrawPolyline(line.ToArray(), false, new Bgr(0, 0, 255), 2);

                foreach (var f in ActiveBall.Frames)
                {
                    Vector2 unproj;

                    if (kinect.ProjectToPx(f.Ball.Position.ToV3(), out unproj))
                    {
                        unproj *= 0.5f;
                        //Vector3 v3debug = f.Ball.Position.ToV3();
                        //string debugtext = "CurrPos - X: " + v3debug.X.ToString("0.00") + " Y: " + v3debug.Y.ToString("0.00") + " Z: " + v3debug.Z.ToString("0.00");
                        //Debug.WriteLine(debugtext);
                        //string debugtext2 = "CurrPosPx - X: " + unproj.X.ToString("0.00") + " Y: " + unproj.Y.ToString("0.00");
                        //Debug.WriteLine(debugtext2);
                        var pt = new System.Drawing.Point((int)unproj.X, (int)unproj.Y);

                        DisplayOut.Draw(new Cross2DF(pt, 5, 5), new Bgr(255, 255, 0), 1);
                    }
                }

            }

   
        }

    }
}
