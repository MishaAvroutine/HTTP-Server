// World map coordinates (simplified for visualization)
const countries = {
    'China': { lat: 35, lon: 105, x: 0, y: 0 },
    'Russia': { lat: 60, lon: 100, x: 0, y: 0 },
    'USA': { lat: 37, lon: -95, x: 0, y: 0 },
    'India': { lat: 20, lon: 78, x: 0, y: 0 },
    'Brazil': { lat: -10, lon: -55, x: 0, y: 0 },
    'Germany': { lat: 51, lon: 10, x: 0, y: 0 },
    'UK': { lat: 54, lon: -2, x: 0, y: 0 },
    'Japan': { lat: 36, lon: 138, x: 0, y: 0 },
    'Australia': { lat: -27, lon: 133, x: 0, y: 0 },
    'Canada': { lat: 56, lon: -106, x: 0, y: 0 }
};

const protocols = ['TCP', 'UDP', 'HTTP', 'HTTPS', 'SSH', 'DNS', 'FTP', 'ICMP'];
const targetServers = ['Primary_Server_01', 'Secondary_Server_02', 'Database_Cluster', 'API_Gateway', 'Admin_Panel', 'VPN_Server'];

let globalAttackCount = 0;
let recentAttacks = [];
let attackAnimations = [];
const canvas = document.getElementById('worldMap');
const ctx = canvas.getContext('2d');
let attacksThisSecond = 0;

// Canvas setup
function setupCanvas() {
    canvas.width = window.innerWidth - 320;
    canvas.height = window.innerHeight - 80;

    // Convert latitude/longitude to canvas coordinates
    Object.keys(countries).forEach(country => {
        const data = countries[country];
        // Simple mercator-like projection
        data.x = ((data.lon + 180) / 360) * canvas.width;
        data.y = ((90 - data.lat) / 180) * canvas.height;
    });
}

// Draw world map with grid and points
function drawMap() {
    ctx.fillStyle = 'rgba(10, 14, 39, 0.8)';
    ctx.fillRect(0, 0, canvas.width, canvas.height);

    // Draw grid
    ctx.strokeStyle = 'rgba(0, 255, 136, 0.1)';
    ctx.lineWidth = 1;
    for (let i = 0; i < canvas.width; i += 50) {
        ctx.beginPath();
        ctx.moveTo(i, 0);
        ctx.lineTo(i, canvas.height);
        ctx.stroke();
    }
    for (let i = 0; i < canvas.height; i += 50) {
        ctx.beginPath();
        ctx.moveTo(0, i);
        ctx.lineTo(canvas.width, i);
        ctx.stroke();
    }

    // Draw country points
    Object.keys(countries).forEach(country => {
        const pos = countries[country];
        // Glow effect
        const gradient = ctx.createRadialGradient(pos.x, pos.y, 0, pos.x, pos.y, 20);
        gradient.addColorStop(0, 'rgba(0, 255, 136, 0.3)');
        gradient.addColorStop(1, 'rgba(0, 255, 136, 0)');
        ctx.fillStyle = gradient;
        ctx.fillRect(pos.x - 20, pos.y - 20, 40, 40);

        // Point
        ctx.fillStyle = '#00ff88';
        ctx.beginPath();
        ctx.arc(pos.x, pos.y, 5, 0, Math.PI * 2);
        ctx.fill();

        // Label
        ctx.fillStyle = '#00ff88';
        ctx.font = 'bold 10px Courier New';
        ctx.textAlign = 'center';
        ctx.fillText(country, pos.x, pos.y + 20);
    });
}

// User's server location (centered)
const userServer = {
    x: canvas.width / 2,
    y: canvas.height / 2,
    label: 'YOUR_SERVER'
};

function drawUserServer() {
    const pos = userServer;
    // Larger glow
    const gradient = ctx.createRadialGradient(pos.x, pos.y, 0, pos.x, pos.y, 50);
    gradient.addColorStop(0, 'rgba(0, 255, 136, 0.5)');
    gradient.addColorStop(1, 'rgba(0, 255, 136, 0)');
    ctx.fillStyle = gradient;
    ctx.fillRect(pos.x - 50, pos.y - 50, 100, 100);

    // Central point
    ctx.fillStyle = '#00ff88';
    ctx.beginPath();
    ctx.arc(pos.x, pos.y, 8, 0, Math.PI * 2);
    ctx.fill();

    // Ring
    ctx.strokeStyle = '#00ff88';
    ctx.lineWidth = 2;
    ctx.beginPath();
    ctx.arc(pos.x, pos.y, 15, 0, Math.PI * 2);
    ctx.stroke();

    // Pulsing outer ring
    const time = Date.now() / 1000;
    const pulse = Math.sin(time * 3) * 10 + 20;
    ctx.strokeStyle = `rgba(0, 255, 136, ${Math.max(0, 0.3 - pulse / 100)})`;
    ctx.lineWidth = 1;
    ctx.beginPath();
    ctx.arc(pos.x, pos.y, pulse, 0, Math.PI * 2);
    ctx.stroke();
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

    // Create attack animation
    attackAnimations.push({
        id: attack.id,
        sourcePos: countries[attack.source],
        targetPos: userServer,
        progress: 0,
        speed: 0.015 + Math.random() * 0.015,
        color: ['#ff006e', '#ff4444', '#ff0050'][Math.floor(Math.random() * 3)]
    });

    return attack;
}

// Draw attack animations
function drawAttackAnimations() {
    for (let i = attackAnimations.length - 1; i >= 0; i--) {
        const attack = attackAnimations[i];
        attack.progress += attack.speed;

        if (attack.progress >= 1) {
            attackAnimations.splice(i, 1);
            continue;
        }

        // Calculate current position
        const x = attack.sourcePos.x + (attack.targetPos.x - attack.sourcePos.x) * attack.progress;
        const y = attack.sourcePos.y + (attack.targetPos.y - attack.sourcePos.y) * attack.progress;

        // Draw trail
        ctx.strokeStyle = attack.color;
        ctx.lineWidth = 2;
        ctx.globalAlpha = 1 - attack.progress;
        ctx.beginPath();
        ctx.moveTo(attack.sourcePos.x, attack.sourcePos.y);
        ctx.lineTo(x, y);
        ctx.stroke();
        ctx.globalAlpha = 1;

        // Draw projectile
        const size = 4 + Math.sin(attack.progress * Math.PI * 4) * 2;
        ctx.fillStyle = attack.color;
        ctx.beginPath();
        ctx.arc(x, y, size, 0, Math.PI * 2);
        ctx.fill();

        // Glow
        ctx.shadowColor = attack.color;
        ctx.shadowBlur = 10;
        ctx.beginPath();
        ctx.arc(x, y, size + 2, 0, Math.PI * 2);
        ctx.stroke();
        ctx.shadowBlur = 0;
    }
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
    drawMap();
    drawUserServer();
    drawAttackAnimations();
    updateAttacksList();

    requestAnimationFrame(animate);
}

// Generate attacks periodically
let attackRate = 2; // attacks per second
function spawnAttacks() {
    const numAttacks = Math.floor(attackRate);
    for (let i = 0; i < numAttacks; i++) {
        generateAttack();
    }
    // Vary attack rate
    if (Math.random() > 0.98) {
        attackRate = 1 + Math.random() * 5;
    }
}

// Setup and start
window.addEventListener('resize', setupCanvas);
setupCanvas();
animate();

// Spawn attacks
setInterval(spawnAttacks, 1000);

// Update statistics less frequently
setInterval(() => {
    updateStatistics();
    attacksThisSecond = 0;
}, 1000);

// Initial data
for (let i = 0; i < 5; i++) {
    generateAttack();
}
updateStatistics();
