package com.barnocraft.raycast;

import com.badlogic.gdx.math.Vector3;

public class RaycastResult {
    public final int x, y, z;
    public final int normalX, normalY, normalZ;
    public final float distance;
    
    public RaycastResult(int x, int y, int z, int normalX, int normalY, int normalZ, float distance) {
        this.x = x;
        this.y = y;
        this.z = z;
        this.normalX = normalX;
        this.normalY = normalY;
        this.normalZ = normalZ;
        this.distance = distance;
    }
    
    public Vector3 getBlockPosition() {
        return new Vector3(x, y, z);
    }
    
    public Vector3 getNormal() {
        return new Vector3(normalX, normalY, normalZ);
    }
    
    public Vector3 getAdjacentBlockPosition() {
        return new Vector3(x + normalX, y + normalY, z + normalZ);
    }
}
