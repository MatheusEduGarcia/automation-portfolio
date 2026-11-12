FROM nodered/node-red:4.0.9
RUN npm install --no-audit --no-fund node-red-dashboard@3.6.5
COPY --chown=node-red:node-red flows.json /data/flows.json
