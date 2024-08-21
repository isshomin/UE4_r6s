using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using MySql.Data.MySqlClient;

namespace RainbowMasterServer.Models
{
    public class ServerData
    {
        public int ServerID { get; set; }
        public string IPAddress { get; set; }
        public string ServerName { get; set; }
        public string MapName { get; set; }
        public int CurrentPlayers { get; set; }
        public int MaxPlayers { get; set; }

    }

}