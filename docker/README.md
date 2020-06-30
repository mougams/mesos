## SETUP
1. `DOCKER_BUILDKIT=1 docker build -f docker/Dockerfile --target runner -t runner-mesos .`
2. `DOCKER_BUILDKIT=1 docker build -f docker/Dockerfile --target builder -t builder-mesos .`
3. `docker run -v $(pwd):/src/mesos builder-mesos`
4. `docker-compose -f docker/docker-compose.yml up`

After each source modification, just kill docker-compose, rebuild and `docker-compose up`

## TODO
 - [ ] Simplify env to put more of them in `/usr/bin/mesos-{agent, master}`
 - [ ] Remove extends to use 3.4 docker-compose only: simple solution is to
   generate the docker-compose with a template engine
 - [ ] Make a marathon image based on centos to leverage freshly built libmesos
   and Criteo's marathon-ui.
 - [ ] Add a proxy in the compose to allow Marathon Healthchecks (not possible
   with localhost logic).
