--深淵の指名者
function c89801755.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetCost(c89801755.cost)
	e1:SetTarget(c89801755.target)
	e1:SetOperation(c89801755.activate)
	c:RegisterEffect(e1)
end
function c89801755.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckLPCost(tp,1000) end
	Duel.PayLPCost(tp,1000)
end
function c89801755.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.Hint(HINT_SELECTMSG,tp,562)
	local att=Duel.AnnounceAttribute(tp,1,0xff)
	Duel.Hint(HINT_SELECTMSG,tp,563)
	local rc=Duel.AnnounceRace(tp,1,0xffffff)
	e:SetLabel(att)
	Duel.SetTargetPlayer(1-tp)
	Duel.SetTargetParam(rc)
	Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,nil,1,1-tp,LOCATION_HAND+LOCATION_GRAVE)
end
function c89801755.filter(c,rc,att)
	return c:IsRace(rc) and c:IsAttribute(att) and c:IsAbleToGrave()
end
function c89801755.activate(e,tp,eg,ep,ev,re,r,rp)
	local att=e:GetLabel()
	local p,rc=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local g=Duel.SelectMatchingCard(p,c89801755.filter,p,LOCATION_HAND+LOCATION_DECK,0,1,1,nil,rc,att)
	if g:GetCount()>0 then
		Duel.SendtoGrave(g,REASON_EFFECT)
	else
		local cg=Duel.GetFieldGroup(p,LOCATION_DECK+LOCATION_HAND,0)
		Duel.ConfirmCards(1-p,cg)
		Duel.ShuffleHand(p)
		Duel.ShuffleDeck(p)
	end
end
