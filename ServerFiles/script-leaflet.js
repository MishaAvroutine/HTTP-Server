// Initialize Leaflet Map
let map;
let markersLayer;
let animationLayer;

// Country coordinates
const countries = {
    'China': { lat: 35, lon: 105 },
    'Russia': { lat: 60, lon: 100 },
    'USA': { lat: 37, lon: -95 },
    'India': { lat: 20, lon: 78 },
    'Brazil': { lat: -10, lon: -55 },
    'Germany': { lat: 51, lon: 10 },
    'UK': { lat: 54, lon: -2 },
    'Japan': { lat: 36, lon: 138 },
    'Australia': { lat: -27, lon: 133 },
    'Canada': { lat: 56, lon: -106 }
};

// Server location (center of map)
const serverLocation = { lat: 20, lon: 0 };

const protocols = ['TCP', 'UDP', 'HTTP', 'HTTPS', 'SSH', 'DNS', 'FTP', 'ICMP'];
const targetServers = ['Primary_Server_01', 'Secondary_Server_02', 'Database_Cluster', 'API_Gateway', 'Admin_Panel', 'VPN_Server'];

let globalAttackCount = 0;
let recentAttacks = [];
let attackAnimations = [];
let attacksThisSecond = 0;

function initializeMap() {
    // Create map centered on Europe/Africa
    map = L.map('leafletMap').setView([serverLocation.lat, serverLocation.lon], 2);

    // Add OpenStreetMap tiles
    L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
        attribution: '© OpenStreetMap contributors',
        maxZoom: 19,
        className: 'map-tiles'
    }).addTo(map);

    // Create layers
    markersLayer = L.featureGroup().addTo(map);
    animationLayer = L.featureGroup().addTo(map);

    // Add server marker at center
    const serverIcon = L.divIcon({
        html: `<div class="server-marker">🖥️</div>`,
        className: 'server-marker-container',
        iconSize: [40, 40],
        iconAnchor: [20, 20]
    });

    L.marker([serverLocation.lat, serverLocation.lon], { icon: serverIcon }).addTo(map);

    // Add country markers
    Object.keys(countries).forEach(country => {
        const pos = countries[country];
        const icon = L.divIcon({
            html: `<div class="country-marker" title="${country}"></div>`,
            className: 'country-marker-container',
            iconSize: [20, 20],
            iconAnchor: [10, 10]
        });

        L.marker([pos.lat, pos.lon], { icon: icon }).addTo(markersLayer)
            .bindPopup(`<strong>${country}</strong><br>Attacks from this region`);
    });
}

// Custom CSS for markers
function addMapStyles() {
    const style = document.createElement('style');
    style.textContent = `
        #leafletMap {
            width: 100%;
            height: 100%;
            background: #0a0e27;
            z-index: 1;
        }

        .leaflet-container {
            background: linear-gradient(135deg, #0a0e27 0%, #16213e 100%);
            font-family: 'Courier New', monospace;
        }

        .leaflet-tile-pane {
            filter: brightness(0.7) contrast(1.2) hue-rotate(180deg);
        }

        .leaflet-control-zoom,
        .leaflet-control-attribution {
            display: none !important;
        }

        .server-marker-container {
            display: flex;
            justify-content: center;
            align-items: center;
            animation: pulse-glow 2s ease-in-out infinite;
        }

        .server-marker {
            font-size: 24px;
            filter: drop-shadow(0 0 10px #00ff88);
            text-shadow: 0 0 15px #00ff88;
        }

        .country-marker-container {
            display: flex;
            justify-content: center;
            align-items: center;
        }

        .country-marker {
            width: 100%;
            height: 100%;
            background: radial-gradient(circle, #ff006e, #ff4444);
            border-radius: 50%;
            box-shadow: 0 0 10px #ff006e;
            cursor: pointer;
            transition: all 0.3s ease;
        }

        .country-marker:hover {
            box-shadow: 0 0 20px #ff006e;
            transform: scale(1.3);
        }

        .attack-line {
            stroke: #ffff00;
            stroke-width: 2;
            fill: none;
            opacity: 0.6;
        }

        .attack-pulse {
            animation: attack-pulse 1.5s ease-in-out infinite;
        }

        @keyframes pulse-glow {
            0%, 100% {
                filter: drop-shadow(0 0 5px #00ff88);
            }
            50% {
                filter: drop-shadow(0 0 20px #00ff88);
            }
        }

        @keyframes attack-pulse {
            0% {
                r: 3;
                opacity: 1;
            }
            100% {
                r: 15;
                opacity: 0;
            }
        }

        .leaflet-popup-content {
            color: #00ff88 !important;
            font-family: 'Courier New', monospace;
            background: rgba(10, 14, 39, 0.95) !important;
            border: 1px solid #00ff88 !important;
        }

        .leaflet-popup-content-wrapper {
            background: rgba(10, 14, 39, 0.95);
            border: 1px solid #00ff88;
            border-radius: 4px;
        }

        .leaflet-popup-tip {
            background: rgba(10, 14, 39, 0.95);
            border: 1px solid #00ff88;
        }
    `;
    document.head.appendChild(style);
}

// Generate random attack
function generateAttack() {
    const originCountry = Object.keys(countries)[Math.floor(Math.random() * Object.keys(countries).length)];
    const protocol = protocols[Math.floor(Math.random() * protocols.length)];
    const target = targetServers[Math.floor(Math.random() * targetServers.length)];
    const ipOctet = `${Math.floor(Math.random() * 256)}.${Math.floor(Math.random() * 256)}.${Math.floor(Math.random() * 256)}.${Math.floor(Math.random() * 256)}`;

    const attack = {
        id: Date.now() + Math.random(),
        source: originCountry,
        sourceIP: ipOctet,
        target: target,
        protocol: protocol,
        timestamp: new Date(),
        latency: Math.floor(Math.random() * 300) + 20,
        packets: Math.floor(Math.random() * 10000) + 100
    };

    globalAttackCount++;
    attacksThisSecond++;
    recentAttacks.unshift(attack);
    if (recentAttacks.length > 15) recentAttacks.pop();

    // Draw attack line on map
    drawAttackLine(attack);

    return attack;
}

// Draw attack animation line
function drawAttackLine(attack) {
    const origin = countries[attack.source];
    const destination = [serverLocation.lat, serverLocation.lon];
    const start = [origin.lat, origin.lon];

    // Draw polyline
    const polyline = L.polyline([start, destination], {
        color: '#ffff00',
        weight: 2,
        opacity: 0.7,
        dashArray: '5, 5',
        className: 'attack-line'
    }).addTo(animationLayer);

    // Animate removal
    setTimeout(() => {
        animationLayer.removeLayer(polyline);
    }, 2000);

    // Add pulsing circle at attack origin
    const pulseCircle = L.circleMarker([origin.lat, origin.lon], {
        radius: 5,
        fillColor: '#ff006e',
        color: '#ff006e',
        weight: 2,
        opacity: 0.8,
        fillOpacity: 0.8,
        className: 'attack-pulse'
    }).addTo(animationLayer);

    setTimeout(() => {
        animationLayer.removeLayer(pulseCircle);
    }, 1500);
}

// Update sidebar with recent attacks
function updateAttacksList() {
    const list = document.getElementById('attacksList');
    list.innerHTML = recentAttacks.slice(0, 8).map(attack => `
        <div class="attack-item">
            <div class="attack-item-source">🔴 ${attack.source}</div>
            <div class="attack-item-target">➜ ${attack.target}</div>
            <div class="attack-item-protocol">
                <div class="protocol-badge">${attack.protocol}</div>
                <div class="protocol-badge">${attack.sourceIP}</div>
            </div>
        </div>
    `).join('');
}

// Update statistics
function updateStatistics() {
    document.getElementById('attacksPerSecond').textContent = attacksThisSecond;
    document.getElementById('totalAttacks').textContent = globalAttackCount.toLocaleString();

    // Count unique IPs
    const uniqueIPs = new Set(recentAttacks.map(a => a.sourceIP)).size;
    document.getElementById('totalIPs').textContent = uniqueIPs;

    // Average latency
    const avgLatency = recentAttacks.length > 0
        ? Math.round(recentAttacks.reduce((sum, a) => sum + a.latency, 0) / recentAttacks.length)
        : 0;
    document.getElementById('avgLatency').textContent = avgLatency + 'ms';

    // Top protocol
    const protocolCount = {};
    recentAttacks.forEach(a => {
        protocolCount[a.protocol] = (protocolCount[a.protocol] || 0) + 1;
    });
    const topProto = Object.keys(protocolCount).length > 0
        ? Object.keys(protocolCount).sort((a, b) => protocolCount[b] - protocolCount[a])[0]
        : 'N/A';
    document.getElementById('topProtocol').textContent = topProto;

    // Top country
    const countryCount = {};
    recentAttacks.forEach(a => {
        countryCount[a.source] = (countryCount[a.source] || 0) + 1;
    });
    const topCountry = Object.keys(countryCount).length > 0
        ? Object.keys(countryCount).sort((a, b) => countryCount[b] - countryCount[a])[0]
        : '--';
    document.getElementById('topCountry').textContent = topCountry;

    // Protocol distribution
    const protocolList = document.getElementById('protocolList');
    const protocolHTML = Object.entries(protocolCount)
        .sort((a, b) => b[1] - a[1])
        .slice(0, 5)
        .map(([proto, count]) => `
            <div style="margin-bottom: 8px;">
                <div style="display: flex; justify-content: space-between; font-size: 11px; margin-bottom: 3px;">
                    <span>${proto}</span>
                    <span style="color: #ff006e;">${count}</span>
                </div>
                <div style="background: rgba(0,255,136,0.2); height: 4px; border-radius: 2px; overflow: hidden;">
                    <div style="background: #ff006e; height: 100%; width: ${(count / Math.max(...Object.values(protocolCount), 1)) * 100}%;"></div>
                </div>
            </div>
        `).join('');
    protocolList.innerHTML = protocolHTML || '<div style="color: #888; font-size: 10px;">No data</div>';
}

// Animation loop
function animate() {
    updateAttacksList();
    requestAnimationFrame(animate);
}

// Generate attacks periodically
let attackRate = 2;
function spawnAttacks() {
    const numAttacks = Math.floor(attackRate);
    for (let i = 0; i < numAttacks; i++) {
        generateAttack();
    }
    if (Math.random() > 0.98) {
        attackRate = 1 + Math.random() * 5;
    }
}

// Initialize
document.addEventListener('DOMContentLoaded', () => {
    addMapStyles();
    initializeMap();
    animate();

    // Spawn attacks
    setInterval(spawnAttacks, 1000);

    // Update statistics
    setInterval(() => {
        updateStatistics();
        attacksThisSecond = 0;
    }, 1000);

    // Initial data
    for (let i = 0; i < 5; i++) {
        generateAttack();
    }
    updateStatistics();
});
