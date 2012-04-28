--電池メン－単二型
function c19733961.initial_effect(c)
	--atk,def
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(LOCATION_MZONE,0)
	e1:SetTarget(aux.TargetBoolFunction(Card.IsRace,RACE_MACHINE))
	e1:SetValue(c19733961.atkval)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EFFECT_UPDATE_DEFENCE)
	e2:SetValue(c19733961.defval)
	c:RegisterEffect(e2)
end
function c19733961.filter(c)
	return c:IsFaceup() and c:IsCode(19733961)
end
function c19733961.atkval(e,c)
	local g=Duel.GetMatchingGroup(c19733961.filter,c:GetControler(),LOCATION_MZONE,0,nil)
	if g:IsExists(Card.IsDefencePos,1,nil) then return 0 end
	return 500
end
function c19733961.defval(e,c)
	local g=Duel.GetMatchingGroup(c19733961.filter,c:GetControler(),LOCATION_MZONE,0,nil)
	if g:IsExists(Card.IsAttackPos,1,nil) then return 0 end
	return 500
end
