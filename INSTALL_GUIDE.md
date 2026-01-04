# ViceversaChain - Guida Installazione e Manutenzione

**Versione:** 1.0.0
**Data:** 2025-12-28
**Basato su:** Bitcoin Core (fork personalizzato)

---

## Indice

1. [Caratteristiche Uniche ViceversaChain](#caratteristiche-uniche-viceversachain)
2. [Requisiti Sistema](#requisiti-sistema)
3. [Installazione](#installazione)
4. [Configurazione](#configurazione)
5. [Avvio e Gestione](#avvio-e-gestione)
6. [Manutenzione](#manutenzione)
7. [Backup e Sicurezza](#backup-e-sicurezza)
8. [Troubleshooting](#troubleshooting)
9. [Comandi Utili](#comandi-utili)

---

## Caratteristiche Uniche ViceversaChain

### Blockchain Inversa
ViceversaChain implementa una blockchain con **altezze inverse**:
- **Genesis Block:** Height 100,000,000 (blocco più vecchio)
- **Blocchi recenti:** Heights decrescenti verso 0
- **Direzione:** 100,000,000 → 99,999,999 → ... → 0

### Parametri Principali
- **Block Time:** 2 minuti
- **Reward:** Raddoppia ogni 1,314,000 blocchi (~5 anni)
- **Genesis Reward:** 0.25 VVC
- **Network:** Mainnet (porta 8333)
- **RPC:** Porta 8332

---

## Requisiti Sistema

### Hardware Minimo
- **CPU:** 2+ cores
- **RAM:** 4 GB
- **Disco:** 50 GB liberi (blockchain + indici)
- **Rete:** Connessione internet stabile

### Hardware Raccomandato
- **CPU:** 4+ cores
- **RAM:** 8 GB
- **Disco:** 100 GB SSD
- **Rete:** 10+ Mbps

### Sistema Operativo
- **Linux:** Ubuntu 20.04+, Debian 11+, Fedora 35+
- **Kernel:** 5.4+

### Dipendenze Software
```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    libtool \
    autotools-dev \
    automake \
    pkg-config \
    bsdmainutils \
    python3 \
    libevent-dev \
    libboost-dev \
    libboost-system-dev \
    libboost-filesystem-dev \
    libboost-test-dev \
    libsqlite3-dev \
    libminiupnpc-dev \
    libnatpmp-dev \
    libzmq3-dev \
    libqrencode-dev \
    libdb5.3++-dev \
    git

# Fedora/RHEL
sudo dnf install -y \
    gcc-c++ \
    libtool \
    make \
    autoconf \
    automake \
    python3 \
    libevent-devel \
    boost-devel \
    libdb-cxx-devel \
    miniupnpc-devel \
    zeromq-devel \
    qrencode-devel \
    sqlite-devel
```

---

## Installazione

### 1. Clona Repository
```bash
cd ~/Scrivania/claudiano
git clone https://github.com/yourorg/viceversachain.git
cd viceversachain
```

### 2. Compila il Codice
```bash
# Genera script di configurazione
./autogen.sh

# Configura build
./configure \
    --without-gui \
    --with-incompatible-bdb \
    --enable-cxx \
    --disable-tests \
    --disable-bench

# Compila (usa tutti i core disponibili)
make -j$(nproc)

# [Opzionale] Installa system-wide
sudo make install
```

### 3. Verifica Installazione
```bash
# Se NON installato system-wide
./src/viceversachaind --version
./src/viceversachain-cli --version

# Se installato system-wide
viceversachaind --version
viceversachain-cli --version
```

**Output atteso:**
```
Viceversachain Core version v1.0.0
Copyright (C) 2025 The Viceversachain Core developers
```

---

## Configurazione

### 1. Crea Directory Dati
```bash
mkdir -p ~/.viceversachain
```

### 2. Crea File di Configurazione
```bash
nano ~/.viceversachain/viceversachain.conf
```

### 3. Configurazione Base
```conf
# Configurazione Mainnet ViceversaChain

# Network
listen=1
maxconnections=125

# RPC
server=1
rpcuser=tuouser
rpcpassword=tuapasswordsicura123!
rpcallowip=127.0.0.1
rpcport=8332

# Performance
dbcache=2048
maxmempool=300

# Logging
debug=0
printtoconsole=0

# Wallet
disablewallet=0

# Indici (raccomandati)
txindex=1
```

### 4. Configurazione Avanzata (Opzionale)
```conf
# Abilita block filter index (per scanblocks)
blockfilterindex=1

# Limita banda upload (MB/giorno, 0=illimitato)
maxuploadtarget=5000

# Prune (riduce spazio disco, incompatibile con txindex)
# prune=10000

# ZMQ notifications (per applicazioni esterne)
# zmqpubhashblock=tcp://127.0.0.1:28332
# zmqpubhashtx=tcp://127.0.0.1:28333

# Logging dettagliato (debug)
# debug=net
# debug=rpc
# debug=mempool
```

### 5. Permessi File
```bash
chmod 600 ~/.viceversachain/viceversachain.conf
```

---

## Avvio e Gestione

### Avvio Daemon

#### Metodo 1: Foreground (per debugging)
```bash
cd ~/Scrivania/claudiano/viceversachain
./src/viceversachaind -conf=$HOME/.viceversachain/viceversachain.conf
```

#### Metodo 2: Background (normale)
```bash
cd ~/Scrivania/claudiano/viceversachain
./src/viceversachaind -daemon -conf=$HOME/.viceversachain/viceversachain.conf
```

### Verifica Stato
```bash
./src/viceversachain-cli getblockchaininfo
./src/viceversachain-cli getnetworkinfo
./src/viceversachain-cli getwalletinfo
```

### Stop Daemon
```bash
# Shutdown pulito (raccomandato)
./src/viceversachain-cli stop

# Oppure kill processo (solo se necessario)
pkill -15 viceversachaind
```

### Servizio Systemd (Avvio Automatico)

Crea file servizio:
```bash
sudo nano /etc/systemd/system/viceversachain.service
```

Contenuto:
```ini
[Unit]
Description=ViceversaChain Daemon
After=network.target

[Service]
Type=forking
User=gianni
Group=gianni
WorkingDirectory=/home/gianni/Scrivania/claudiano/viceversachain
ExecStart=/home/gianni/Scrivania/claudiano/viceversachain/src/viceversachaind \
    -daemon \
    -conf=/home/gianni/.viceversachain/viceversachain.conf \
    -pid=/home/gianni/.viceversachain/viceversachaind.pid

ExecStop=/home/gianni/Scrivania/claudiano/viceversachain/src/viceversachain-cli stop

Restart=on-failure
RestartSec=10
TimeoutStopSec=120

# Limiti risorse
LimitNOFILE=8192

[Install]
WantedBy=multi-user.target
```

Abilita e avvia:
```bash
sudo systemctl daemon-reload
sudo systemctl enable viceversachain.service
sudo systemctl start viceversachain.service

# Verifica stato
sudo systemctl status viceversachain.service

# Logs
sudo journalctl -u viceversachain.service -f
```

---

## Manutenzione

### Sincronizzazione Iniziale

Al primo avvio, il nodo deve sincronizzare tutta la blockchain:

```bash
# Monitora progresso
watch -n 5 './src/viceversachain-cli getblockchaininfo | grep -E "(blocks|headers|verificationprogress)"'

# Oppure
./src/viceversachain-cli getblockchaininfo | jq '.verificationprogress'
```

**Tempo stimato:** 6-24 ore (dipende da rete e hardware)

### Aggiornamento Software

```bash
# Stop daemon
./src/viceversachain-cli stop

# Backup wallet
cp ~/.viceversachain/wallets/aaa/wallet.dat ~/backup_wallet_$(date +%Y%m%d).dat

# Update codice
cd ~/Scrivania/claudiano/viceversachain
git pull origin main

# Ricompila
make clean
./autogen.sh
./configure --without-gui --with-incompatible-bdb
make -j$(nproc)

# Riavvia
./src/viceversachaind -daemon
```

### Verifica Integrità Blockchain

```bash
# Verifica ultimi 288 blocchi (~1 giorno)
./src/viceversachain-cli verifychain 3 288

# Verifica completa (LENTO, può richiedere ore)
./src/viceversachain-cli verifychain 4 0
```

### Rebuild Indici

Se txindex o altri indici sono corrotti:

```bash
# Stop daemon
./src/viceversachain-cli stop

# Riavvia con reindex
./src/viceversachaind -daemon -reindex

# Monitora progresso
tail -f ~/.viceversachain/debug.log
```

### Pulizia Mempool

```bash
# Salva stato mempool
./src/viceversachain-cli savemempool

# Informazioni mempool
./src/viceversachain-cli getmempoolinfo
```

### Monitoraggio Risorse

```bash
# Spazio disco
du -sh ~/.viceversachain/

# Dettaglio directory
du -h --max-depth=1 ~/.viceversachain/ | sort -hr

# Uso memoria
ps aux | grep viceversachaind

# Connessioni rete
./src/viceversachain-cli getconnectioncount
./src/viceversachain-cli getpeerinfo | jq '.[].addr'
```

---

## Backup e Sicurezza

### Backup Wallet

```bash
# Metodo 1: Comando RPC
./src/viceversachain-cli backupwallet ~/backup_wallet_$(date +%Y%m%d_%H%M%S).dat

# Metodo 2: Copia manuale (daemon DEVE essere fermo)
./src/viceversachain-cli stop
cp ~/.viceversachain/wallets/aaa/wallet.dat ~/backup_wallet.dat
./src/viceversachaind -daemon

# Metodo 3: Dump chiavi private
./src/viceversachain-cli dumpwallet ~/wallet_dump_$(date +%Y%m%d).txt
chmod 600 ~/wallet_dump_*.txt
```

### Restore Wallet

```bash
# Stop daemon
./src/viceversachain-cli stop

# Restore da backup
./src/viceversachaind -daemon
./src/viceversachain-cli restorewallet "restored_wallet" ~/backup_wallet.dat

# Oppure importa dump
./src/viceversachain-cli importwallet ~/wallet_dump_20251228.txt
```

### Crittografia Wallet

```bash
# Cripta wallet (PRIMA VOLTA)
./src/viceversachain-cli encryptwallet "tuaPassphraseMoltoSicura123!"
# NOTA: Daemon si riavvierà automaticamente

# Sblocca wallet temporaneamente (600 secondi)
./src/viceversachain-cli walletpassphrase "tuaPassphraseMoltoSicura123!" 600

# Cambia passphrase
./src/viceversachain-cli walletpassphrasechange "vecchiaPass" "nuovaPass"

# Blocca wallet
./src/viceversachain-cli walletlock
```

### Sicurezza Sistema

```bash
# Firewall - Permetti solo porta P2P
sudo ufw allow 8333/tcp
sudo ufw enable

# RPC deve rimanere locale (già in config: rpcallowip=127.0.0.1)
# NON esporre porta 8332 pubblicamente!

# Permessi directory
chmod 700 ~/.viceversachain
chmod 600 ~/.viceversachain/viceversachain.conf
chmod 600 ~/.viceversachain/wallets/aaa/wallet.dat
```

---

## Troubleshooting

### Daemon Non Si Avvia

```bash
# Verifica lock file
rm -f ~/.viceversachain/.lock

# Verifica log errori
tail -100 ~/.viceversachain/debug.log

# Avvia in foreground per vedere errori
./src/viceversachaind -printtoconsole

# Verifica porte occupate
netstat -tulpn | grep -E '(8332|8333)'
```

### Sincronizzazione Bloccata

```bash
# Verifica connessioni
./src/viceversachain-cli getconnectioncount
./src/viceversachain-cli getpeerinfo

# Aggiungi nodi manualmente (se necessario)
./src/viceversachain-cli addnode "94.156.35.84:8333" "add"

# Riavvia con refresh peers
./src/viceversachain-cli stop
rm ~/.viceversachain/peers.dat
./src/viceversachaind -daemon
```

### Wallet Locked Error

```bash
# Se wallet è crittografato, sbloccalo prima di operazioni che richiedono chiavi
./src/viceversachain-cli walletpassphrase "tuaPassphrase" 300

# Poi esegui comando (es. send)
./src/viceversachain-cli sendtoaddress "VCwFdYx..." 1.5
```

### Errore "Insufficient Funds"

```bash
# Verifica balance
./src/viceversachain-cli getbalance
./src/viceversachain-cli getbalances

# Verifica UTXO disponibili
./src/viceversachain-cli listunspent

# Verifica transazioni in attesa
./src/viceversachain-cli listtransactions "*" 100
```

### Database Corrotto

```bash
# Stop daemon
./src/viceversachain-cli stop

# Backup dati
cp -r ~/.viceversachain ~/.viceversachain_backup_$(date +%Y%m%d)

# Reindex completo
./src/viceversachaind -daemon -reindex

# Se fallisce, reindex-chainstate (più veloce)
./src/viceversachaind -daemon -reindex-chainstate
```

### Problemi Specifici ViceversaChain

#### Rescan Blockchain Non Funziona
```bash
# BUG NOTO: rescanblockchain ha validazione height errata
# Workaround: usa -rescan al riavvio daemon

./src/viceversachain-cli stop
./src/viceversachaind -daemon -rescan
```

#### GetBlockStats Genesis Fallisce
```bash
# BUG NOTO: getblockstats 100000000 fallisce con "Can't read undo data"
# Non è possibile ottenere stats del genesis block
# Workaround: usa blocchi più recenti (< 100000000)

./src/viceversachain-cli getblockstats 99996343  # OK
./src/viceversachain-cli getblockstats 100000000  # FALLISCE
```

---

## Comandi Utili

### Informazioni Generali

```bash
# Versione
./src/viceversachain-cli --version

# Stato blockchain
./src/viceversachain-cli getblockchaininfo

# Network info
./src/viceversachain-cli getnetworkinfo

# Uptime nodo
./src/viceversachain-cli uptime

# Mining info
./src/viceversachain-cli getmininginfo
```

### Wallet

```bash
# Balance
./src/viceversachain-cli getbalance
./src/viceversachain-cli getbalances

# Nuovo indirizzo
./src/viceversachain-cli getnewaddress "label"

# Lista transazioni
./src/viceversachain-cli listtransactions "*" 50

# Invia VVC
./src/viceversachain-cli sendtoaddress "indirizzo" 1.5

# UTXO
./src/viceversachain-cli listunspent
```

### Blockchain Query

```bash
# Blocco corrente
./src/viceversachain-cli getblockcount

# Info blocco
./src/viceversachain-cli getblock "hash" 2

# Transazione
./src/viceversachain-cli getrawtransaction "txid" true

# Chain tips
./src/viceversachain-cli getchaintips

# Stats blockchain
./src/viceversachain-cli getchaintxstats 144
```

### Mempool

```bash
# Transazioni in mempool
./src/viceversachain-cli getrawmempool

# Info mempool
./src/viceversachain-cli getmempoolinfo

# Dettaglio tx in mempool
./src/viceversachain-cli getmempoolentry "txid"
```

### Network

```bash
# Peer connessi
./src/viceversachain-cli getpeerinfo

# Numero connessioni
./src/viceversachain-cli getconnectioncount

# Nodi noti
./src/viceversachain-cli getnodeaddresses 10

# Ban IP
./src/viceversachain-cli setban "IP" "add" 86400

# Lista ban
./src/viceversachain-cli listbanned
```

### Manutenzione

```bash
# Verifica chain
./src/viceversachain-cli verifychain

# Info indici
./src/viceversachain-cli getindexinfo

# Salva mempool
./src/viceversachain-cli savemempool

# RPC info
./src/viceversachain-cli getrpcinfo

# Logs
tail -f ~/.viceversachain/debug.log
```

---

## Script Utili

### Monitor Automatico

Crea `~/monitor_vvc.sh`:
```bash
#!/bin/bash

echo "=== ViceversaChain Status ==="
echo ""

CLI="$HOME/Scrivania/claudiano/viceversachain/src/viceversachain-cli"

echo "Blockchain:"
$CLI getblockchaininfo | jq '{blocks, headers, verificationprogress, size_on_disk}'

echo ""
echo "Network:"
$CLI getnetworkinfo | jq '{version, subversion, connections, networkactive}'

echo ""
echo "Wallet:"
$CLI getbalances | jq '.mine'

echo ""
echo "Mempool:"
$CLI getmempoolinfo | jq '{size, bytes, usage}'

echo ""
echo "Uptime: $($CLI uptime) seconds"
```

Usa:
```bash
chmod +x ~/monitor_vvc.sh
~/monitor_vvc.sh
```

### Backup Automatico

Crea `~/backup_vvc.sh`:
```bash
#!/bin/bash

CLI="$HOME/Scrivania/claudiano/viceversachain/src/viceversachain-cli"
BACKUP_DIR="$HOME/vvc_backups"
DATE=$(date +%Y%m%d_%H%M%S)

mkdir -p "$BACKUP_DIR"

echo "Backing up wallet..."
$CLI backupwallet "$BACKUP_DIR/wallet_$DATE.dat"

echo "Backup saved to: $BACKUP_DIR/wallet_$DATE.dat"

# Mantieni solo ultimi 30 backup
ls -t "$BACKUP_DIR"/wallet_*.dat | tail -n +31 | xargs -r rm

echo "Done. Total backups: $(ls "$BACKUP_DIR"/wallet_*.dat | wc -l)"
```

Usa:
```bash
chmod +x ~/backup_vvc.sh

# Cron giornaliero (ogni giorno alle 2:00 AM)
crontab -e
# Aggiungi: 0 2 * * * /home/gianni/backup_vvc.sh
```

---

## Risorse

### Directory Importanti
- **Config:** `~/.viceversachain/viceversachain.conf`
- **Wallet:** `~/.viceversachain/wallets/aaa/wallet.dat`
- **Blockchain:** `~/.viceversachain/blocks/`
- **Chainstate:** `~/.viceversachain/chainstate/`
- **Logs:** `~/.viceversachain/debug.log`
- **Indexes:** `~/.viceversachain/indexes/txindex/`

### File di Lock
- `~/.viceversachain/.lock` - Lock directory
- `~/.viceversachain/.cookie` - RPC auth cookie
- `~/.viceversachain/viceversachaind.pid` - Process ID

### Documentazione
- **RPC Test Status:** `/home/gianni/Scrivania/claudiano/viceversachain/RPC_TEST_STATUS.md`
- **Bitcoin Core Docs:** https://bitcoin.org/en/developer-documentation
- **ViceversaChain GitHub:** [inserire URL]

---

## Support

Per problemi o bug:
1. Verifica `~/.viceversachain/debug.log`
2. Controlla `RPC_TEST_STATUS.md` per bug noti
3. Apri issue su GitHub con log completi

**IMPORTANTE:** Alcuni comandi RPC hanno bug noti (vedi `RPC_TEST_STATUS.md`):
- `rescanblockchain` - Validazione height errata (non fixato)
- `getblockstats 100000000` - Genesis block fail (limitazione)

---

**Ultima Revisione:** 2025-12-28
**Autore:** ViceversaChain Team
**Licenza:** MIT
