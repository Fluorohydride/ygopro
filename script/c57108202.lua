--D・リモコン
function c57108202.initial_effect(c)
	--search
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(57108202,0))
	e1:SetCategory(CATEGORY_REMOVE+CATEGORY_TOHAND+CATEGORY_SEARCH)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCountLimit(1)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCondition(c57108202.cona)
	e1:SetTarget(c57108202.tga)
	e1:SetOperation(c57108202.opa)
	c:RegisterEffect(e1)
	--salvage
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(57108202,1))
	e2:SetCategory(CATEGORY_TOGRAVE+CATEGORY_TOHAND)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetCountLimit(1)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCondition(c57108202.cond)
	e2:SetTarget(c57108202.tgd)
	e2:SetOperation(c57108202.opd)
	c:RegisterEffect(e2)
end
function c57108202.filter(c,lv)
	return c:IsSetCard(0x26) and c:GetLevel()==lv and c:IsAbleToHand()
end
function c57108202.cona(e,tp,eg,ep,ev,re,r,rp)
	return not e:GetHandler():IsDisabled() and e:GetHandler():IsAttackPos()
end
function c57108202.filtera(c,tp)
	local lv=c:GetLevel()
	return c:IsSetCard(0x26) and lv>0 and c:IsAbleToRemove()
		and Duel.IsExistingMatchingCard(c57108202.filter,tp,LOCATION_DECK,0,1,nil,lv)
end
function c57108202.tga(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c57108202.filtera(chkc,tp) end
	if chk==0 then return Duel.IsExistingTarget(c57108202.filtera,tp,LOCATION_GRAVE,0,1,nil,tp) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g=Duel.SelectTarget(tp,c57108202.filtera,tp,LOCATION_GRAVE,0,1,1,nil,tp)
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,g,1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK)
end
function c57108202.opa(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and Duel.Remove(tc,POS_FACEUP,REASON_EFFECT)~=0 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
		local g=Duel.SelectMatchingCard(tp,c57108202.filter,tp,LOCATION_DECK,0,1,1,nil,tc:GetLevel())
		if g:GetCount()>0 then
			Duel.SendtoHand(g,nil,REASON_EFFECT)
			Duel.ConfirmCards(1-tp,g)
		end
	end
end
function c57108202.cond(e,tp,eg,ep,ev,re,r,rp)
	return not e:GetHandler():IsDisabled() and e:GetHandler():IsDefencePos()
end
function c57108202.filterd(c,tp)
	local lv=c:GetLevel()
	return c:IsSetCard(0x26) and lv>0 and c:IsAbleToGrave()
		and Duel.IsExistingMatchingCard(c57108202.filter,tp,LOCATION_GRAVE,0,1,nil,lv)
end
function c57108202.tgd(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c57108202.filterd,tp,LOCATION_HAND,0,1,nil,tp) end
	Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,nil,1,tp,LOCATION_HAND)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_GRAVE)
end
function c57108202.opd(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local g=Duel.SelectMatchingCard(tp,c57108202.filterd,tp,LOCATION_HAND,0,1,1,nil,tp)
	local tc=g:GetFirst()
	if not tc then return end
	Duel.SendtoGrave(tc,REASON_EFFECT)
	if not tc:IsLocation(LOCATION_GRAVE) then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local sg=Duel.SelectMatchingCard(tp,c57108202.filter,tp,LOCATION_GRAVE,0,1,1,tc,tc:GetLevel())
	if sg:GetCount()>0 then
		Duel.SendtoHand(sg,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,sg)
	end
end
