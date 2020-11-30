FROM python:3.8-slim

WORKDIR /app
COPY index.html opencv.js ./

CMD ["python3", "-m", "http.server", "8003"]
