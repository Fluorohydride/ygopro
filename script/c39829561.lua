--D－HERO ディパーテッドガイ
function c39829561.initial_effect(c)
	--Special Summon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(39829561,0))
	e1:SetType(EFFECT_TYPE_TRIGGER_F+EFFECT_TYPE_FIELD)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetCode(EVENT_PHASE+PHASE_STANDBY)
	e1:SetRange(LOCATION_GRAVE)
	e1:SetCountLimit(1)
	e1:SetCondition(c39829561.condition)
	e1:SetTarget(c39829561.target)
	e1:SetOperation(c39829561.operation)
	c:RegisterEffect(e1)
	--redirect
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_TO_GRAVE_REDIRECT)
	e2:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e2:SetCondition(c39829561.recon)
	e2:SetValue(LOCATION_REMOVED)
	c:RegisterEffect(e2)
end
function c39829561.filter(c)
	return c:IsFaceup() and c:IsCode(83986578)
end
function c39829561.condition(e,tp,eg,ep,ev,re,r,rp)
	return tp==Duel.GetTurnPlayer() and not Duel.IsExistingMatchingCard(c39829561.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil) 
end
function c39829561.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c39829561.operation(e,tp,eg,ep,ev,re,r,rp)
	if e:GetHandler():IsRelateToEffect(e) then
		Duel.SpecialSummon(e:GetHandler(),0,tp,1-tp,false,false,POS_FACEUP_ATTACK)
	end
end
function c39829561.recon(e)
	local c=e:GetHandler()
	return (c:IsLocation(LOCATION_MZONE) and c:IsReason(REASON_BATTLE))
		or (c:IsLocation(LOCATION_DECK+LOCATION_HAND) and c:IsReason(REASON_EFFECT))
end
