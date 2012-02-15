--光神テテュス
function c87148330.initial_effect(c)
	--draw
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(87148330,0))
	e1:SetProperty(EFFECT_FLAG_DAMAGE_STEP+EFFECT_FLAG_PLAYER_TARGET)
	e1:SetRange(LOCATION_MZONE)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_DRAW)
	e1:SetCost(c87148330.drcost)
	e1:SetTarget(c87148330.drtg)
	e1:SetOperation(c87148330.drop)
	c:RegisterEffect(e1)
end
function c87148330.filter(c)
	return c:IsRace(RACE_FAIRY) and not c:IsPublic()
end
function c87148330.drcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return ep==tp and eg:IsExists(c87148330.filter,1,nil) end
	local g=eg:Filter(c87148330.filter,nil)
	if g:GetCount()==1 then
		Duel.ConfirmCards(1-tp,g)
		Duel.ShuffleHand(tp)
	else
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_CONFIRM)
		local sg=g:Select(tp,1,1,nil)
		Duel.ConfirmCards(1-tp,sg)
		Duel.ShuffleHand(tp)
	end
end
function c87148330.drtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsPlayerCanDraw(tp,1) end
	Duel.SetTargetPlayer(tp)
	Duel.SetTargetParam(1)
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,1)
end
function c87148330.drop(e,tp,eg,ep,ev,re,r,rp)
	if e:GetHandler():IsFacedown() or not e:GetHandler():IsRelateToEffect(e) then return end
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Draw(p,d,REASON_EFFECT)
end
