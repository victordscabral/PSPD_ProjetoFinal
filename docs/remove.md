# Remover instalação (caso necessário)

```bash
sudo kubeadm reset -f
sudo apt-get purge -y kubeadm kubectl kubelet kubernetes-cni
sudo apt-get autoremove -y
sudo rm -rf ~/.kube
sudo rm -rf /etc/kubernetes/
sudo rm -rf /var/lib/etcd
sudo rm -rf /var/lib/cni/
sudo rm -rf /var/lib/kubelet/*
sudo rm -rf /etc/cni/net.d/*
sudo iptables -F && sudo iptables -X
sudo ipvsadm --clear
sudo systemctl restart containerd
sudo systemctl restart kubelet
```

```bash
sudo systemctl stop docker containerd
sudo apt-get remove -y docker docker.io containerd containerd.io
sudo apt-get autoremove -y
sudo rm -rf /var/lib/docker
sudo rm -rf /var/lib/containerd
sudo rm -rf /etc/docker
```

```bash
sudo apt-mark unhold kubeadm kubectl kubelet
sudo apt-get purge -y kubeadm kubectl kubelet kubernetes-cni
sudo apt-get autoremove -y
```

```bash
sudo dpkg --purge containerd.io docker.io
sudo apt-get autoremove -y
sudo rm -rf /var/lib/docker
sudo rm -rf /var/lib/containerd
sudo rm -rf /etc/docker
```

```bash
sudo reboot
```

Depois seguir instalação do [download2.md](download2.md)