--無差別破壊
function c32015116.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_END_PHASE)
	c:RegisterEffect(e1)
	--roll and destroy
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(32015116,0))
	e2:SetCategory(CATEGORY_DESTROY+CATEGORY_DICE)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCountLimit(1)
	e2:SetCode(EVENT_PHASE+PHASE_STANDBY)
	e2:SetCondition(c32015116.rdcon)
	e2:SetTarget(c32015116.rdtg)
	e2:SetOperation(c32015116.rdop)
	c:RegisterEffect(e2)
end
function c32015116.rdcon(e,tp,eg,ep,ev,re,r,rp)
	return tp==Duel.GetTurnPlayer()
end
function c32015116.rdtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_DICE,nil,0,tp,1)
end
function c32015116.rdfilter(c,lv)
	if lv<6 then
		return c:IsFaceup() and c:IsDestructable() and c:GetLevel()==lv
	else
		return c:IsFaceup() and c:IsDestructable() and c:GetLevel()>=6 end
end
function c32015116.rdop(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local d1=Duel.TossDice(tp,1)
	local g=Duel.GetMatchingGroup(c32015116.rdfilter,tp,LOCATION_MZONE,LOCATION_MZONE,nil,d1)
	Duel.Destroy(g,REASON_EFFECT)
end
