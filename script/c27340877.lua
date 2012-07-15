--DNA定期健診
function c27340877.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DRAW)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c27340877.target)
	e1:SetOperation(c27340877.activate)
	c:RegisterEffect(e1)
end
function c27340877.filter(c)
	return c:IsFacedown() and c:GetRace()~=0
end
function c27340877.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(tp) and c27340877.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c27340877.filter,tp,LOCATION_MZONE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEDOWN)
	Duel.SelectTarget(tp,c27340877.filter,tp,LOCATION_MZONE,0,1,1,nil)
	Duel.Hint(HINT_SELECTMSG,1-tp,562)
	local rc=Duel.AnnounceAttribute(1-tp,2,0xffffff)
	e:SetLabel(rc)
end
function c27340877.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and tc:IsFacedown() then
		Duel.ConfirmCards(1-tp,tc)
		if tc:IsAttribute(e:GetLabel()) then
			Duel.Draw(1-tp,2,REASON_EFFECT)
		else
			Duel.Draw(tp,2,REASON_EFFECT)
		end
	end
end
