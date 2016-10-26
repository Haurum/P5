using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Sockets;
using System.Runtime.Serialization.Formatters.Binary;
using System.Text;
using System.Threading.Tasks;

namespace WifiTest
{
    class Program
    {
        static TcpClient tcpClient = new TcpClient();

        static void Main(string[] args)
        {
            tcpClient.Connect("192.168.0.100", 9999);
            object data = "Test send";
            const int IntSize = 4;
            byte[] bytedata;

            using (MemoryStream ms = new MemoryStream())
            {
                BinaryFormatter bf = new BinaryFormatter();

                try { bf.Serialize(ms, data); }
                catch { return; }

                bytedata = ms.ToArray();
            }
            tcpClient.Client.BeginSend(BitConverter.GetBytes(bytedata.Length), 0, IntSize, SocketFlags.None, EndSend, null);
        }

        public static void EndSend(IAsyncResult ar)
        {
            try { tcpClient.Client.EndSend(ar); }
            catch { }
        }
    }
}
