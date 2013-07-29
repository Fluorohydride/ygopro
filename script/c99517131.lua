--生命力吸収魔術
function c99517131.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_POSITION+CATEGORY_RECOVER)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_DRAW_PHASE)
	e1:SetTarget(c99517131.target)
	e1:SetOperation(c99517131.activate)
	c:RegisterEffect(e1)
end
function c99517131.filter(c)
	return c:IsFaceup() and c:IsType(TYPE_EFFECT)
end
function c99517131.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(nil,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil) end
	local rec=Duel.GetMatchingGroupCount(c99517131.filter,tp,LOCATION_MZONE,LOCATION_MZONE,nil)*400
	Duel.SetTargetPlayer(tp)
	Duel.SetTargetParam(rec)
	if rec>0 then Duel.SetOperationInfo(0,CATEGORY_RECOVER,nil,0,tp,rec) end
end
function c99517131.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(Card.IsFacedown,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
	Duel.ChangePosition(g,POS_FACEUP_ATTACK,POS_FACEUP_ATTACK,POS_FACEUP_DEFENCE,POS_FACEUP_DEFENCE,true)
	local p=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER)
	local rec=Duel.GetMatchingGroupCount(c99517131.filter,tp,LOCATION_MZONE,LOCATION_MZONE,nil)*400
	Duel.Recover(p,rec,REASON_EFFECT)
end
