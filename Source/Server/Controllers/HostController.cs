using System.Data;
using System.Web.Http;
using RainbowMasterServer.Models;

namespace RainbowMasterServer.Controllers
{
    public class HostController : ApiController
    {
        // GET: api/Host
        public DataTable Get()
        {
            DatabaseInterface DB = new DatabaseInterface();
            return DB.GetAllServers();
        }

        // GET: api/Host/5
        public string Get(int id)
        {
             return "value";
        }

        // POST: api/Host
        public int Post(ServerData Data)
        {
            DatabaseInterface DB = new DatabaseInterface();
            return DB.PostData(Data);
        }

        // PUT: api/Host/5
        public void Put(int id, ServerData Data)
        {
            DatabaseInterface DB = new DatabaseInterface();
            DB.UpdateServerEntry(Data);
        }

        // DELETE: api/Host/5
        public void Delete(int id)
        {
            DatabaseInterface DB = new DatabaseInterface();
            DB.DeleteData();
        }
    }
}
