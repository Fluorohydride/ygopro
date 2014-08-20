--スピリット変換装置
function c20802187.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--material
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(20802187,0))
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_SZONE)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetCountLimit(1)
	e2:SetTarget(c20802187.mattg)
	e2:SetOperation(c20802187.matop)
	c:RegisterEffect(e2)
	--salvage
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(20802187,1))
	e3:SetCategory(CATEGORY_TOHAND)
	e3:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e3:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e3:SetCode(EVENT_TO_GRAVE)
	e3:SetCondition(c20802187.thcon)
	e3:SetTarget(c20802187.thtg)
	e3:SetOperation(c20802187.thop)
	c:RegisterEffect(e3)
end
function c20802187.xyzfilter(c)
	return c:IsFaceup() and c:IsType(TYPE_XYZ)
end
function c20802187.matfilter(c)
	return c:IsFaceup() and c:IsAttribute(ATTRIBUTE_LIGHT) and c:GetLevel()==4 and not c:IsType(TYPE_TOKEN)
end
function c20802187.mattg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(tp) and chkc:IsLocation(LOCATION_MZONE) and c20802187.xyzfilter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c20802187.xyzfilter,tp,LOCATION_MZONE,0,1,nil)
		and Duel.IsExistingMatchingCard(c20802187.matfilter,tp,LOCATION_MZONE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	Duel.SelectTarget(tp,c20802187.xyzfilter,tp,LOCATION_MZONE,0,1,1,nil)
end
function c20802187.matop(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local tc=Duel.GetFirstTarget()
	if tc:IsFaceup() and tc:IsRelateToEffect(e) and not tc:IsImmuneToEffect(e) then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
		local g=Duel.SelectMatchingCard(tp,c20802187.matfilter,tp,LOCATION_MZONE,0,1,1,nil)
		if g:GetCount()>0 then
			Duel.Overlay(tc,g)
		end
	end
end
function c20802187.thcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():IsPreviousLocation(LOCATION_ONFIELD)
end
function c20802187.thfilter(c,e)
	return c:IsRace(RACE_THUNDER) and c:IsAttribute(ATTRIBUTE_LIGHT) and c:GetLevel()==4 and c:IsCanBeEffectTarget(e)
end
function c20802187.thfilter2(c,g)
	return g:IsExists(Card.IsCode,1,c,c:GetCode())
end
function c20802187.thtg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(tp) and chkc:IsLocation(LOCATION_GRAVE) and c20802187.thfilter(chkc,e) end
	local g=Duel.GetMatchingGroup(c20802187.thfilter,tp,LOCATION_GRAVE,0,nil,e)
	if chk==0 then return true end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g1=g:FilterSelect(tp,c20802187.thfilter2,1,1,nil,g)
	if g1:GetCount()>0 then
		local g2=g:FilterSelect(tp,Card.IsCode,1,1,g1:GetFirst(),g1:GetFirst():GetCode())
		g1:Merge(g2)
		Duel.SetTargetCard(g1)
		Duel.SetOperationInfo(0,CATEGORY_TOHAND,g1,2,0,0)
	end
end
function c20802187.thop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	if not g then return end
	local sg=g:Filter(Card.IsRelateToEffect,nil,e)
	if sg:GetCount()>0 then
		Duel.SendtoHand(sg,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,sg)
	end
end
