--D－HERO ダイヤモンドガイ
function c13093792.initial_effect(c)
	--
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(13093792,0))
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetCountLimit(1)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTarget(c13093792.target)
	e1:SetOperation(c13093792.operation)
	c:RegisterEffect(e1)
end
function c13093792.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFieldGroupCount(tp,LOCATION_DECK,0)>0 end
	Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,nil,1,0,0)
end
function c13093792.operation(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetFieldGroupCount(tp,LOCATION_DECK,0)==0 then return end
	Duel.ConfirmDecktop(tp,1)
	local g=Duel.GetDecktopGroup(tp,1)
	local tc=g:GetFirst()
	if tc:GetType()==TYPE_SPELL then
		Duel.DisableShuffleCheck()
		Duel.SendtoGrave(g,REASON_EFFECT)
		local ae=tc:GetActivateEffect()
		if tc:GetLocation()==LOCATION_GRAVE and ae then
			local e1=Effect.CreateEffect(tc)
			e1:SetDescription(ae:GetDescription())
			e1:SetType(EFFECT_TYPE_IGNITION)
			e1:SetCountLimit(1)
			e1:SetRange(LOCATION_GRAVE)
			e1:SetReset(RESET_EVENT+0x2fe0000+RESET_PHASE+PHASE_END+RESET_SELF_TURN,2)
			e1:SetCondition(c13093792.spellcon)
			e1:SetTarget(c13093792.spelltg)
			e1:SetOperation(c13093792.spellop)
			tc:RegisterEffect(e1)
		end
	else
		Duel.MoveSequence(tc,1)
	end
end
function c13093792.spellcon(e,tp,eg,ep,ev,re,r,rp,chk)
	return e:GetHandler():GetTurnID()~=Duel.GetTurnCount()
end
function c13093792.spelltg(e,tp,eg,ep,ev,re,r,rp,chk)
	local ae=e:GetHandler():GetActivateEffect()
	local ftg=ae:GetTarget()
	if chk==0 then
		return not ftg or ftg(e,tp,eg,ep,ev,re,r,rp,chk)
	end
	if ae:IsHasProperty(EFFECT_FLAG_CARD_TARGET) then
		e:SetProperty(EFFECT_FLAG_CARD_TARGET)
	else e:SetProperty(0) end
	if ftg then
		ftg(e,tp,eg,ep,ev,re,r,rp,chk)
	end
end
function c13093792.spellop(e,tp,eg,ep,ev,re,r,rp)
	local ae=e:GetHandler():GetActivateEffect()
	local fop=ae:GetOperation()
	fop(e,tp,eg,ep,ev,re,r,rp)
end
