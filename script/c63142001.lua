--電池メン－単三型
function c63142001.initial_effect(c)
	--atk,def
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetValue(c63142001.atkval)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EFFECT_UPDATE_DEFENCE)
	e2:SetValue(c63142001.defval)
	c:RegisterEffect(e2)
end
function c63142001.filter(c)
	return c:IsFaceup() and c:IsCode(63142001)
end
function c63142001.atkval(e,c)
	local g=Duel.GetMatchingGroup(c63142001.filter,c:GetControler(),LOCATION_MZONE,0,nil)
	if g:IsExists(Card.IsDefencePos,1,nil) then return 0 end
	return g:GetCount()*1000
end
function c63142001.defval(e,c)
	local g=Duel.GetMatchingGroup(c63142001.filter,c:GetControler(),LOCATION_MZONE,0,nil)
	if g:IsExists(Card.IsAttackPos,1,nil) then return 0 end
	return g:GetCount()*1000
end
