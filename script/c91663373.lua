--サイバー・エスパー
function c91663373.initial_effect(c)
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(91663373,0))
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EVENT_DRAW)
	e1:SetCondition(c91663373.cfcon)
	e1:SetOperation(c91663373.cfop)
	c:RegisterEffect(e1)
end
function c91663373.cfcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():IsAttackPos() and ep==1-tp
end
function c91663373.filter(c)
	return c:IsLocation(LOCATION_HAND) and not c:IsPublic()
end
function c91663373.cfop(e,tp,eg,ep,ev,re,r,rp)
	if e:GetHandler():IsRelateToEffect(e) and e:GetHandler():IsPosition(POS_FACEUP_ATTACK) then
		local cg=eg:Filter(c91663373.filter,nil)
		Duel.ConfirmCards(tp,cg)
		Duel.ShuffleHand(1-tp)
	end
end
