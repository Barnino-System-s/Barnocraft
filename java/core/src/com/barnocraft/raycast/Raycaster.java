package com.barnocraft.raycast;

import com.badlogic.gdx.math.Vector3;
import com.barnocraft.Constants;
import com.barnocraft.world.World;

public class Raycaster {
    private final World world;
    
    public Raycaster(World world) {
        this.world = world;
    }
    
    public RaycastResult raycast(Vector3 origin, Vector3 direction, float maxDistance) {
        int x = (int) Math.floor(origin.x);
        int y = (int) Math.floor(origin.y);
        int z = (int) Math.floor(origin.z);
        
        int stepX = direction.x >= 0 ? 1 : -1;
        int stepY = direction.y >= 0 ? 1 : -1;
        int stepZ = direction.z >= 0 ? 1 : -1;
        
        float tDeltaX = direction.x != 0 ? Math.abs(1f / direction.x) : Float.POSITIVE_INFINITY;
        float tDeltaY = direction.y != 0 ? Math.abs(1f / direction.y) : Float.POSITIVE_INFINITY;
        float tDeltaZ = direction.z != 0 ? Math.abs(1f / direction.z) : Float.POSITIVE_INFINITY;
        
        float tMaxX = direction.x >= 0 ? 
            (x + 1 - origin.x) * tDeltaX : 
            (origin.x - x) * tDeltaX;
        float tMaxY = direction.y >= 0 ? 
            (y + 1 - origin.y) * tDeltaY : 
            (origin.y - y) * tDeltaY;
        float tMaxZ = direction.z >= 0 ? 
            (z + 1 - origin.z) * tDeltaZ : 
            (origin.z - z) * tDeltaZ;
        
        int normalX = 0, normalY = 0, normalZ = 0;
        float distance = 0;
        
        while (distance <= maxDistance) {
            if (world.isInBounds(x, y, z) && world.isSolid(x, y, z)) {
                return new RaycastResult(x, y, z, normalX, normalY, normalZ, distance);
            }
            
            if (tMaxX < tMaxY && tMaxX < tMaxZ) {
                x += stepX;
                distance = tMaxX;
                tMaxX += tDeltaX;
                normalX = -stepX;
                normalY = 0;
                normalZ = 0;
            } else if (tMaxY < tMaxZ) {
                y += stepY;
                distance = tMaxY;
                tMaxY += tDeltaY;
                normalX = 0;
                normalY = -stepY;
                normalZ = 0;
            } else {
                z += stepZ;
                distance = tMaxZ;
                tMaxZ += tDeltaZ;
                normalX = 0;
                normalY = 0;
                normalZ = -stepZ;
            }
        }
        
        return null;
    }
}
