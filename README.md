# hpc_job_power_predictionpacman -S docker

```bash
groupadd docker
sudo usermod -aG docker ${USER}

docker info
# reboot

systemctl start docker.service
systemctl enable docker.service

docker build --tag hpc:1.0 .

docker images
```
