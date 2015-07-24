--禁忌の壺
function c91501248.initial_effect(c)
	--flip
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_FLIP+EFFECT_TYPE_TRIGGER_O)
	e1:SetProperty(EFFECT_FLAG_DELAY)
	e1:SetCountLimit(1,91501248)
	e1:SetTarget(c91501248.target)
	e1:SetOperation(c91501248.operation)
	c:RegisterEffect(e1)
end
function c91501248.thfilter(c)
	return c:IsType(TYPE_SPELL+TYPE_TRAP) and c:IsAbleToHand()
end
function c91501248.target(e,tp,eg,ep,ev,re,r,rp,chk)
	local b1=Duel.IsPlayerCanDraw(tp,2)
	local b2=Duel.IsExistingMatchingCard(c91501248.thfilter,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,nil)
	local b3=Duel.IsExistingMatchingCard(Card.IsDestructable,tp,0,LOCATION_MZONE,1,nil)
	local b4=Duel.IsExistingMatchingCard(Card.IsAbleToDeck,tp,0,LOCATION_HAND,1,nil)
	if chk==0 then return b1 or b2 or b3 or b4 end
	local off=1
	local ops={}
	local opval={}
	if b1 then
		ops[off]=aux.Stringid(91501248,0)
		opval[off-1]=1
		off=off+1
	end
	if b2 then
		ops[off]=aux.Stringid(91501248,1)
		opval[off-1]=2
		off=off+1
	end
	if b3 then
		ops[off]=aux.Stringid(91501248,2)
		opval[off-1]=3
		off=off+1
	end
	if b4 then
		ops[off]=aux.Stringid(91501248,3)
		opval[off-1]=4
		off=off+1
	end
	local op=Duel.SelectOption(tp,table.unpack(ops))
	local sel=opval[op]
	e:SetLabel(sel)
	if sel==1 then
		e:SetCategory(CATEGORY_DRAW)
		Duel.SetTargetPlayer(tp)
		Duel.SetTargetParam(2)
		Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,2)
	elseif sel==2 then
		e:SetCategory(CATEGORY_TOHAND)
		local g=Duel.GetMatchingGroup(c91501248.thfilter,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,nil)
		Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,g:GetCount(),0,0)
	elseif sel==3 then
		e:SetCategory(CATEGORY_DESTROY)
		local g=Duel.GetMatchingGroup(Card.IsDestructable,tp,0,LOCATION_MZONE,nil)
		Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
	else
		e:SetCategory(CATEGORY_TODECK)
		Duel.SetTargetPlayer(tp)
		Duel.SetOperationInfo(0,CATEGORY_TODECK,nil,0,1-tp,LOCATION_HAND)
	end
end
function c91501248.operation(e,tp,eg,ep,ev,re,r,rp)
	local sel=e:GetLabel()
	if sel==1 then
		local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
		Duel.Draw(p,d,REASON_EFFECT)
	elseif sel==2 then
		local g=Duel.GetMatchingGroup(c91501248.thfilter,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,nil)
		Duel.SendtoHand(g,nil,REASON_EFFECT)
	elseif sel==3 then
		local g=Duel.GetMatchingGroup(Card.IsDestructable,tp,0,LOCATION_MZONE,nil)
		Duel.Destroy(g,REASON_EFFECT)
	else
		local p=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER)
		local g=Duel.GetFieldGroup(p,0,LOCATION_HAND)
		if g:GetCount()>0 then
			Duel.ConfirmCards(p,g)
			Duel.Hint(HINT_SELECTMSG,p,HINTMSG_TODECK)
			local sg=g:FilterSelect(p,Card.IsAbleToDeck,1,1,nil)
			Duel.SendtoDeck(sg,nil,2,REASON_EFFECT)
			Duel.ShuffleHand(1-p)
		end
	end
end
