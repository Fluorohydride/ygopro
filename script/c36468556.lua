--停戦協定
function c36468556.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_POSITION+CATEGORY_DAMAGE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_DRAW_PHASE)
	e1:SetTarget(c36468556.target)
	e1:SetOperation(c36468556.activate)
	c:RegisterEffect(e1)
end
function c36468556.tgfilter(c)
	return (c:IsFaceup() and c:IsType(TYPE_EFFECT)) or c:IsFacedown()
end
function c36468556.filter(c)
	return c:IsFaceup() and c:IsType(TYPE_EFFECT)
end
function c36468556.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c36468556.tgfilter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil) end
	local dam=Duel.GetMatchingGroupCount(c36468556.filter,tp,LOCATION_MZONE,LOCATION_MZONE,nil)*500
	Duel.SetTargetPlayer(1-tp)
	Duel.SetTargetParam(dam)
	if dam>0 then Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,dam) end
end
function c36468556.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(Card.IsFacedown,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
	Duel.ChangePosition(g,0x1,0x1,0x4,0x4,true)
	local p=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER)
	local dam=Duel.GetMatchingGroupCount(c36468556.filter,tp,LOCATION_MZONE,LOCATION_MZONE,nil)*500
	Duel.Damage(p,dam,REASON_EFFECT)
end
