--ピューマン
function c38837163.initial_effect(c)
	c:EnableReviveLimit()
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_HAND)
	e1:SetCondition(c38837163.spcon)
	e1:SetOperation(c38837163.spop)
	c:RegisterEffect(e1)
	--effect
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(38837163,0))
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCountLimit(1)
	e2:SetCost(c38837163.cost)
	e2:SetTarget(c38837163.target)
	e2:SetOperation(c38837163.operation)
	c:RegisterEffect(e2)
end
function c38837163.spfilter(c)
	return c:IsRace(RACE_BEASTWARRIOR) and c:IsAbleToRemoveAsCost()
end
function c38837163.spcon(e,c)
	if c==nil then return true end
	local tp=c:GetControler()
	return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c38837163.spfilter,tp,LOCATION_GRAVE,0,2,nil)
end
function c38837163.spop(e,tp,eg,ep,ev,re,r,rp,c)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g=Duel.SelectMatchingCard(tp,c38837163.spfilter,tp,LOCATION_GRAVE,0,2,2,nil)
	Duel.Remove(g,POS_FACEUP,REASON_COST)
end
function c38837163.cfilter(c)
	return c:IsRace(RACE_BEASTWARRIOR) and c:IsDiscardable()
end
function c38837163.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c38837163.cfilter,tp,LOCATION_HAND,0,1,nil) end
	Duel.DiscardHand(tp,c38837163.cfilter,1,1,REASON_COST+REASON_DISCARD)
end
function c38837163.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local opt=Duel.SelectOption(tp,aux.Stringid(38837163,1),aux.Stringid(38837163,2))
	e:SetLabel(opt)
end
function c38837163.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if not c:IsRelateToEffect(e) or c:IsFacedown() then return end
	if e:GetLabel()==0 then
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_SET_ATTACK_FINAL)
		e1:SetValue(c:GetAttack()*2)
		e1:SetReset(RESET_EVENT+0x1ff0000+RESET_PHASE+PHASE_END)
		c:RegisterEffect(e1)
	else
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_DIRECT_ATTACK)
		e1:SetReset(RESET_EVENT+0x1ff0000+RESET_PHASE+PHASE_END)
		c:RegisterEffect(e1)
	end
end
