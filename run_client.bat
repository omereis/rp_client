docker rm -f rp_client
docker build --rm -f rp_client.dockerfile -t rp_client .
rem docker run -it -d -h rp_client --link rsa --name rp_client -p 5005:5005 rp_client
docker run -it -d -h rp_client --name rp_client -p 5005:5005 rp_client
rem docker run -it -d --name docker_refsrv
docker exec -it rp_client bash
