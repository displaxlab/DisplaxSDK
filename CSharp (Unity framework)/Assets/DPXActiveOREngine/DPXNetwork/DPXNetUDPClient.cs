using System;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using UnityEngine;
using DPXUtils;

namespace DPXNetwork
{
    public class DPXNetUDPClient
    {

        public delegate void OnConnected();
        public static event OnConnected onConnected;

        public delegate void OnDisconnected();
        public static event OnDisconnected onDisconnected;

        public delegate void OnReadyRead();
        public static event OnReadyRead onReadyRead;

        #region private members 
        private static UdpClient socketConnection = new UdpClient();
        private const int bufSize = 8 * 1024;
        private int localPort = 0;
        private IPAddress localHost = IPAddress.Any;
        private int remotePort = 45001;
        private IPAddress remoteHost = IPAddress.Parse("127.0.0.1");

        private IPEndPoint localEP;
        private IPEndPoint remoteEP;

        private EndPoint EPfrom;

        private AsyncCallback recv = null;

        private class State
        {
            public byte[] buffer = new byte[bufSize];
            public int length = 0;
        }

        private State udpState = new State();
        #endregion

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

        public byte[] DataBuffer()
        {
            return udpState.buffer;
        }

        public int DataLength()
        {
            return udpState.length;
        }

        public void InitUDPConnection()
        {
            localEP = new IPEndPoint(localHost, localPort);
            remoteEP = new IPEndPoint(remoteHost, remotePort);

            EPfrom = new IPEndPoint(localHost, localPort);

            socketConnection.ExclusiveAddressUse = false;
            socketConnection.Client.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, true);
            socketConnection.Client.Bind(localEP);
        }

        public void CloseUDPConnection()
        {
            socketConnection.Client.Close();
            if (onDisconnected != null)
                onDisconnected();
        }

        public void SendUDPMessage(byte[] ux, bool useListenCallback=true)
        {
            if (!socketConnection.Client.IsBound) // try to init connection
            {
                socketConnection.Client.Close();
                InitUDPConnection();
            }
            socketConnection.Client.Connect(remoteEP);
            if (useListenCallback)
                ListenForUDPData();
            try
            {
                socketConnection.Client.BeginSend(ux, 0, ux.Length, SocketFlags.None, (ar) =>
                {
                    State so = (State)ar.AsyncState;
                    int bytes = socketConnection.Client.EndSend(ar);
                    if (onConnected != null)
                        onConnected();
                }, udpState);
            }
            catch {
                CloseUDPConnection();
            }
        }

        private void ListenForUDPData()
        {
            try
            {
                socketConnection.Client.BeginReceiveFrom(udpState.buffer, 0, bufSize, SocketFlags.None, ref EPfrom, recv = (ar) =>
                {
                    // assuming responses are smaller than bufSize (no multi-packets)
                    State so = (State)ar.AsyncState;
                    int bytes = socketConnection.Client.EndReceiveFrom(ar, ref EPfrom);
                    if (DPXConstants.DPX_DEBUG_INFO)
                        Debug.Log("received UDP message");
                    udpState.buffer = so.buffer;
                    udpState.length=bytes;
                    if (onReadyRead != null)
                        onReadyRead();

                }, udpState);
            }
            catch {
                CloseUDPConnection();
            }
        }
    }
}
