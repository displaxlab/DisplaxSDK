using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using UnityEngine;
using DPXUtils;

namespace DPXNetwork
{
    public class DataBuffer
    {
        public byte[] WriteBuffer;
        public int CurrentWriteCount;
        public byte[] ReadBuffer;
        public int CurrentReadCount;

        ~DataBuffer()
        {
            Array.Clear(WriteBuffer, 0, WriteBuffer.Length);
            Array.Clear(ReadBuffer, 0, ReadBuffer.Length);
        }
    }

    public class DPXNetTCPClient
    {
        public delegate void OnConnected();
        public static event OnConnected onConnected;

        public delegate void OnDisconnected();
        public static event OnDisconnected onDisconnected;

        public delegate void OnReadyRead();
        public static event OnReadyRead onReadyRead;

        #region private members 
        private TcpClient tcpClient;
        private NetworkStream clientStream;
        private DataBuffer buffer;
        private int writeBufferSize = 16 * 1024;
        private int readBufferSize = 16 * 1024;
        private int remotePort = 45001;
        private IPAddress remoteHost = IPAddress.Parse("127.0.0.1");
        private bool started = false;

        #endregion

        public bool mustStop { get; set; }

        public string GetRemoteHost()
        {
            return remoteHost.ToString();
        }

        public void SetRemoteHost(string host)
        {
            remoteHost = IPAddress.Parse(host);
        }

        public int GetRemotePort()
        {
            return remotePort;
        }

        public void SetRemotePort(int port)
        {
            remotePort = port;
        }

        public byte[] DataBuffer()    // property
        {
            return buffer.ReadBuffer;
        }

        public int DataLength()    // property
        {
            return buffer.CurrentReadCount;
        }

        /// <summary>
        /// Constructs a new client
        /// </summary>
        public DPXNetTCPClient()
        {
            mustStop = false;
            buffer = new DataBuffer();
            buffer.WriteBuffer = new byte[writeBufferSize];
            buffer.ReadBuffer = new byte[readBufferSize];
            buffer.CurrentWriteCount = 0;
            buffer.CurrentReadCount = 0;
        }

        /// <summary>
        /// Initiates a TCP connection to a TCP server with a given address and port
        /// </summary>
        /// <param name="host">The IP address (IPV4) of the server</param>
        /// <param name="port">The port the server is listening on</param>
        public void ConnectToTcpServer(string host="", int port=0)
        {
            if (port>0)
                remotePort = port;
            
            if (host!="")
                remoteHost = IPAddress.Parse(host);

            if (!mustStop)
            {
                tcpClient = new TcpClient(remoteHost.ToString(), remotePort);
                clientStream = tcpClient.GetStream();

                Thread t = new Thread(new ThreadStart(ListenForPackets));
                started = true;
                t.Start();
                if (onConnected != null)
                    onConnected();
            }
        }

        public int GetCurrentLocalPort()
        {
            if (tcpClient != null && tcpClient.Connected)
                return ((IPEndPoint)tcpClient.Client.LocalEndPoint).Port;
            else
                return -1;
        }

        /// <summary>
        /// This method runs on its own thread, and is responsible for
        /// receiving data from the server and raising an event when data
        /// is received
        /// </summary>
        private void ListenForPackets()
        {
            int bytesRead;
             
            while (started && !mustStop)
            {
                bytesRead = 0;
                QJsonDocTranslator jsonTranslator = new QJsonDocTranslator();
                try
                {
                    //Blocks until a message is received from the server
                    bytesRead = clientStream.Read(buffer.ReadBuffer, 0, readBufferSize);
                    if (DPXConstants.DPX_DEBUG_INFO)
                        Debug.Log("Received data: " + jsonTranslator.JsonFromBinaryData(buffer.ReadBuffer, bytesRead));             
                }
                catch
                {
                    //A socket error has occurred
                    if (DPXConstants.DPX_DEBUG_INFO)
                        Debug.Log("A socket error has occurred with the client socket " + tcpClient.ToString());

                    break;
                }

                buffer.CurrentReadCount = bytesRead;

                if (bytesRead == 0)
                {
                    //The server has disconnected
                    if (DPXConstants.DPX_DEBUG_INFO)
                        Debug.Log("Server disconnected");

                    break;
                }
                else
                {
                    //Send off notifications for delegates of other classes to handle data
                    if (onReadyRead != null)
                        onReadyRead();
                }
                Thread.Sleep(20);
            }

            started = false;
            Disconnect();
        }

        /// <summary>
        /// Adds data to the packet to be sent out, but does not send it across the network
        /// </summary>
        /// <param name="data">The data to be sent</param>
        public void AddToPacket(byte[] data)
        {
            if (!IsConnected())
                return;

            if (buffer.CurrentWriteCount + data.Length > buffer.WriteBuffer.Length)
                FlushData();

            Array.ConstrainedCopy(data, 0, buffer.WriteBuffer, buffer.CurrentWriteCount, data.Length);
            buffer.CurrentWriteCount += data.Length;
        }

        /// <summary>
        /// Flushes all outgoing data to the server
        /// </summary>
        public void FlushData()
        {
            if (!IsConnected() || mustStop)
                return;

            clientStream.Write(buffer.WriteBuffer, 0, buffer.CurrentWriteCount);
            clientStream.Flush();
            buffer.CurrentWriteCount = 0;
        }

        /// <summary>
        /// Sends the byte array data immediately to the server
        /// </summary>
        /// <param name="data"></param>
        public void SendImmediate(byte[] data)
        {
            if (!IsConnected() || mustStop)
                return;

            AddToPacket(data);
            FlushData();
        }

        /// <summary>
        /// Tells whether we're connected to the server
        /// </summary>
        /// <returns></returns>
        public bool IsConnected()
        {
            return started && tcpClient.Connected;
        }

        /// <summary>
        /// Disconnect from the server
        /// </summary>
        public void Disconnect()
        {
            if (DPXConstants.DPX_DEBUG_INFO)
                Debug.Log("Disconnected from server");

            if (tcpClient == null)
                return;

            tcpClient.Close();

            started = false;

            if (onDisconnected != null)
                onDisconnected();
        }
    }
}