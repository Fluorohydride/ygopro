--トゥーン・ドラゴン・エッガー
function c38369349.initial_effect(c)
	c:EnableReviveLimit()
	--special summon
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_SPSUMMON_PROC)
	e2:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e2:SetRange(LOCATION_HAND)
	e2:SetCondition(c38369349.spcon)
	e2:SetOperation(c38369349.spop)
	c:RegisterEffect(e2)
	--destroy
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e3:SetRange(LOCATION_MZONE)
	e3:SetCode(EVENT_LEAVE_FIELD)
	e3:SetCondition(c38369349.sdescon)
	e3:SetOperation(c38369349.sdesop)
	c:RegisterEffect(e3)
	--direct attack
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_SINGLE)
	e4:SetCode(EFFECT_DIRECT_ATTACK)
	e4:SetCondition(c38369349.dircon)
	c:RegisterEffect(e4)
	local e5=Effect.CreateEffect(c)
	e5:SetType(EFFECT_TYPE_SINGLE)
	e5:SetCode(EFFECT_CANNOT_SELECT_BATTLE_TARGET)
	e5:SetCondition(c38369349.atcon)
	e5:SetValue(c38369349.atlimit)
	c:RegisterEffect(e5)
	local e6=Effect.CreateEffect(c)
	e6:SetType(EFFECT_TYPE_SINGLE)
	e6:SetCode(EFFECT_CANNOT_DIRECT_ATTACK)
	e6:SetCondition(c38369349.atcon)
	c:RegisterEffect(e6)
	--cannot attack
	local e7=Effect.CreateEffect(c)
	e7:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e7:SetCode(EVENT_SPSUMMON_SUCCESS)
	e7:SetOperation(c38369349.atklimit)
	c:RegisterEffect(e7)
	--attack cost
	local e8=Effect.CreateEffect(c)
	e8:SetType(EFFECT_TYPE_SINGLE)
	e8:SetCode(EFFECT_ATTACK_COST)
	e8:SetCost(c38369349.atcost)
	e8:SetOperation(c38369349.atop)
	c:RegisterEffect(e8)
end
function c38369349.cfilter(c)
	return c:IsFaceup() and c:IsCode(15259703)
end
function c38369349.spcon(e,c)
	if c==nil then return true end
	local tp=c:GetControler()
	return Duel.IsExistingMatchingCard(c38369349.cfilter,tp,LOCATION_ONFIELD,0,1,nil) 
		and Duel.GetLocationCount(tp,LOCATION_MZONE)>-2 and Duel.CheckReleaseGroup(tp,nil,2,nil)
end
function c38369349.spop(e,tp,eg,ep,ev,re,r,rp,c)
	local tp=c:GetControler()
	local g=Duel.SelectReleaseGroup(tp,nil,2,2,nil)
	Duel.Release(g,REASON_COST)
end
function c38369349.sfilter(c)
	return c:IsReason(REASON_DESTROY) and c:IsPreviousPosition(POS_FACEUP) and c:GetPreviousCodeOnField()==15259703 and c:IsPreviousLocation(LOCATION_ONFIELD)
end
function c38369349.sdescon(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c38369349.sfilter,1,nil)
end
function c38369349.sdesop(e,tp,eg,ep,ev,re,r,rp)
	Duel.Destroy(e:GetHandler(),REASON_EFFECT)
end
function c38369349.atkfilter(c)
	return c:IsFaceup() and c:IsType(TYPE_TOON)
end
function c38369349.dircon(e)
	return not Duel.IsExistingMatchingCard(c38369349.atkfilter,e:GetHandlerPlayer(),0,LOCATION_MZONE,1,nil)
end
function c38369349.atcon(e)
	return Duel.IsExistingMatchingCard(c38369349.atkfilter,e:GetHandlerPlayer(),0,LOCATION_MZONE,1,nil)
end
function c38369349.atlimit(e,c)
	return not c:IsType(TYPE_TOON) or c:IsFacedown()
end
function c38369349.atklimit(e,tp,eg,ep,ev,re,r,rp)
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_CANNOT_ATTACK)
	e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
	e:GetHandler():RegisterEffect(e1)
end
function c38369349.atcost(e,c,tp)
	return Duel.CheckLPCost(tp,500)
end
function c38369349.atop(e,tp,eg,ep,ev,re,r,rp)
	Duel.PayLPCost(tp,500)
end
