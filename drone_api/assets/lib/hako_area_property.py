class HakoAreaProperty:
    def __init__(self, wind_velocity=None, temperature=None):
        """
        エリアのプロパティを定義するクラス。
        
        Args:
            wind_velocity (tuple): (x, y, z) で表す風速 (m/s)。デフォルトは None。
            temperature (float): 摂氏で表す温度。デフォルトは None。
        """
        self.wind_velocity = wind_velocity
        self.temperature = temperature
    
    def get_wind_velocity(self):
        """
        風速を取得するメソッド。
        
        Returns:
            tuple: (x, y, z) で表す風速 (m/s)
        """
        if self.wind_velocity is None:
            return None
        return tuple(self.wind_velocity)
    
    def get_temperature(self):
        """
        温度を取得するメソッド。
        
        Returns:
            float: 摂氏で表す温度
        """
        return self.temperature
    
    def set_wind_velocity(self, wind_velocity):
        """
        風速を設定するメソッド。
        
        Args:
            wind_velocity (tuple): (x, y, z) で表す風速 (m/s)
        """
        self.wind_velocity = wind_velocity
    
    def set_temperature(self, temperature):
        """
        温度を設定するメソッド。
        
        Args:
            temperature (float): 摂氏で表す温度
        """
        self.temperature = temperature
