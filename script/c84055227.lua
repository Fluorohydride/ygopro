--ゲイシャドウ
function c84055227.initial_effect(c)
	c:EnableCounterPermit(0x3001)
	--add counter
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(84055227,0))
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_BATTLE_DESTROYING)
	e1:SetCondition(c84055227.condition)
	e1:SetOperation(c84055227.operation)
	c:RegisterEffect(e1)
	--attackup
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCode(EFFECT_UPDATE_ATTACK)
	e2:SetValue(c84055227.attackup)
	c:RegisterEffect(e2)
end
function c84055227.condition(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	return c:IsRelateToBattle() and c:GetBattleTarget():IsType(TYPE_MONSTER)
end
function c84055227.operation(e,tp,eg,ep,ev,re,r,rp)
	e:GetHandler():AddCounter(0x3001,1)
end
function c84055227.attackup(e,c)
	return c:GetCounter(0x3001)*200
end
