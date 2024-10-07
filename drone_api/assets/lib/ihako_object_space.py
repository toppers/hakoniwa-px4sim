from abc import ABC, abstractmethod

class IHakoObjectSpace(ABC):
    @abstractmethod
    def get_position(self):
        """
        オブジェクトの中心位置を取得するメソッド。
        
        Returns:
            tuple: オブジェクトの中心位置 (x, y, z)
        """
        pass
    
    @abstractmethod
    def get_bounds(self):
        """
        オブジェクトのAABBまたは他の方式での境界情報を取得するメソッド。
        
        Returns:
            dict: 境界情報を表す辞書（例: min, max など）
        """
        pass
