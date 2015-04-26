--魔轟神獣コカトル
function c26704411.initial_effect(c)
	--draw
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(26704411,0))
	e1:SetCategory(CATEGORY_DRAW)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_BATTLE_DESTROYING)
	e1:SetCondition(aux.bdogcon)
	e1:SetCost(c26704411.cost)
	e1:SetTarget(c26704411.tg)
	e1:SetOperation(c26704411.op)
	c:RegisterEffect(e1)
end
function c26704411.costfilter(c)
	return c:IsSetCard(0x35) and c:IsDiscardable()
end
function c26704411.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c26704411.costfilter,tp,LOCATION_HAND,0,1,nil) end
	Duel.DiscardHand(tp,c26704411.costfilter,1,1,REASON_DISCARD+REASON_COST)
end
function c26704411.tg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsPlayerCanDraw(tp,1) end
	Duel.SetTargetPlayer(tp)
	Duel.SetTargetParam(1)
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,1)
end
function c26704411.op(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Draw(p,d,REASON_EFFECT)
end
