# Guia de Instalação

## 1. Configurando o ambiente
```bash
sudo apt-get update && sudo apt-get upgrade
```

```bash
sudo apt install docker.io -y
```

```bash
sudo systemctl enable docker
sudo systemctl start docker
```

```bash
sudo swapoff -a
sudo sed -i '/ swap / s/^\(.*\)$/#\1/g' /etc/fstab
```

```bash
sudo tee /etc/modules-load.d/containerd.conf <<EOF
overlay
br_netfilter
EOF
sudo modprobe overlay
sudo modprobe br_netfilter
```

```bash
sudo tee /etc/sysctl.d/kubernetes.conf <<EOF
net.bridge.bridge-nf-call-ip6tables = 1
net.bridge.bridge-nf-call-iptables = 1
net.ipv4.ip_forward = 1
EOF
```

```bash
sudo sysctl --system
```

## 2. Instalando o containerd

```bash
sudo apt-get install apt-transport-https ca-certificates curl software-properties-common -y
```

```bash
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo gpg --dearmour -o /etc/apt/trusted.gpg.d/docker.gpg
sudo add-apt-repository "deb [arch=amd64] https://download.docker.com/linux/ubuntu $(lsb_release -cs) stable"
```

```bash
sudo apt update
sudo apt install containerd.io
```

```bash
containerd config default | sudo tee /etc/containerd/config.toml >/dev/null
sudo sed -i 's/SystemdCgroup = false/SystemdCgroup = true/' /etc/containerd/config.toml
```

```bash
sudo systemctl restart containerd
sudo systemctl enable containerd
```

## 3. Instalando Kubernetes

```bash
curl -fsSL https://pkgs.k8s.io/core:/stable:/v1.30/deb/Release.key | sudo gpg --dearmor -o /etc/apt/keyrings/kubernetes-apt-keyring.gpg
```

```bash
echo 'deb [signed-by=/etc/apt/keyrings/kubernetes-apt-keyring.gpg] https://pkgs.k8s.io/core:/stable:/v1.30/deb/ /' | sudo tee /etc/apt/sources.list.d/kubernetes.list
```

```bash
sudo apt-get update
sudo apt-get install -y kubelet=1.30.10-1.1 kubeadm=1.30.10-1.1 kubectl=1.30.10-1.1
```

```bash
sudo apt-mark hold kubelet kubeadm kubectl
```

```bash
sudo systemctl enable --now kubelet
```

</br>
</br>
</br>
</br>

## 4. Kubernetes cluster
### 4.1 Master
```bash
sudo kubeadm init --apiserver-advertise-address=192.168.1.10 --pod-network-cidr=10.244.0.0/16
```

```bash
rm -rf $HOME/.kube
mkdir -p $HOME/.kube
sudo cp -i /etc/kubernetes/admin.conf $HOME/.kube/config
sudo chown $(id -u):$(id -g) $HOME/.kube/config
```

```bash
kubectl get nodes
```

```bash
kubectl apply -f https://github.com/flannel-io/flannel/releases/latest/download/kube-flannel.yml
```

```bash
kubectl taint nodes --all node-role.kubernetes.io/control-plane-
```

```bash
kubectl get pods -n kube-system
```

```bash
kubectl run your_kubernetes_app --image=your_docker_image
```

```bash
kubectl get pods
```

### 4.2 Adicionando workers no nó master

```bash
kubeadm join 192.168.1.10:6443 --token lhig5m.wqsmhisg7yyc7ffb \
 --discovery-token-ca-cert-hash sha256:ffbc95629e305b4de054a5de5edf9aa253a93f87bcb2f79a7d84363f539355b4
```

### 4.3 Resetar Kubernetes

```bash
sudo kubeadm reset -f
```

```bash
sudo rm -rf /etc/cni/net.d/*
sudo rm -rf /var/lib/cni/
```

```bash
sudo systemctl restart containerd
sudo systemctl restart kubelet
```

```bash
sudo kubeadm init --apiserver-advertise-address=192.168.1.10 --pod-network-cidr=10.244.0.0/16
```
