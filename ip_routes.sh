                                                                      
IP=$(hostname -I | cut -d" " -f1)
sudo iptables -t nat -I PREROUTING -p tcp -d $IP --dport 8081 -j DNAT --to-destination 127.0.0.1:8081
sudo sysctl -w net.ipv4.conf.all.route_localnet=1
sudo iptables -t nat -A OUTPUT --dst $IP -p tcp --dport 8081 -j DNAT --to-destination 127.0.0.1:8081


