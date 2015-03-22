--トレード・イン
function c38120068.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DRAW)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c38120068.cost)
	e1:SetTarget(c38120068.target)
	e1:SetOperation(c38120068.activate)
	c:RegisterEffect(e1)
end
function c38120068.filter(c)
	return c:GetLevel()==8 and c:IsDiscardable()
end
function c38120068.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c38120068.filter,tp,LOCATION_HAND,0,1,nil) end
	Duel.DiscardHand(tp,c38120068.filter,1,1,REASON_COST+REASON_DISCARD)
end
function c38120068.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsPlayerCanDraw(tp,2) end
	Duel.SetTargetPlayer(tp)
	Duel.SetTargetParam(2)
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,2)
end
function c38120068.activate(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Draw(p,d,REASON_EFFECT)
end
