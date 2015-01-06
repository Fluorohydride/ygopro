--サイバー・ウロボロス
function c30042158.initial_effect(c)
	--draw
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(30042158,0))
	e1:SetCategory(CATEGORY_DRAW)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET+EFFECT_FLAG_CVAL_CHECK)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_REMOVE)
	e1:SetCost(c30042158.cost)
	e1:SetTarget(c30042158.target)
	e1:SetOperation(c30042158.operation)
	e1:SetValue(c30042158.valcheck)
	c:RegisterEffect(e1)
end
function c30042158.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		if Duel.GetFlagEffect(tp,30042158)==0 then
			Duel.RegisterFlagEffect(tp,30042158,RESET_CHAIN,0,1)
			c30042158[0]=Duel.GetMatchingGroupCount(Card.IsAbleToGraveAsCost,tp,LOCATION_HAND,0,nil)
			c30042158[1]=0
		end
		return c30042158[0]-c30042158[1]>=1
	end
	Duel.DiscardHand(tp,Card.IsAbleToGraveAsCost,1,1,REASON_COST)
end
function c30042158.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsPlayerCanDraw(tp,1) end
	Duel.SetTargetPlayer(tp)
	Duel.SetTargetParam(1)
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,1)
end
function c30042158.operation(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Draw(p,d,REASON_EFFECT)
end
function c30042158.valcheck(e)
	c30042158[1]=c30042158[1]+1
end
