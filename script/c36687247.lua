--TG ラッシュ・ライノ
function c36687247.initial_effect(c)
	--atkup
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCondition(c36687247.atcon)
	e1:SetValue(400)
	c:RegisterEffect(e1)
	--to grave
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EVENT_TO_GRAVE)
	e2:SetOperation(c36687247.regop)
	c:RegisterEffect(e2)
end
function c36687247.atcon(e)
	local ph=Duel.GetCurrentPhase()
	return (ph==PHASE_DAMAGE or ph==PHASE_DAMAGE_CAL) and e:GetHandler()==Duel.GetAttacker()
end
function c36687247.regop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if bit.band(c:GetPreviousLocation(),LOCATION_ONFIELD)~=0 and c:IsReason(REASON_DESTROY) then
		local e1=Effect.CreateEffect(c)
		e1:SetDescription(aux.Stringid(36687247,0))
		e1:SetCategory(CATEGORY_TOHAND+CATEGORY_SEARCH)
		e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
		e1:SetCode(EVENT_PHASE+PHASE_END)
		e1:SetCountLimit(1)
		e1:SetRange(LOCATION_GRAVE)
		e1:SetTarget(c36687247.thtg)
		e1:SetOperation(c36687247.thop)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
		c:RegisterEffect(e1)
	end
end
function c36687247.filter(c)
	return c:IsSetCard(0x27) and c:GetCode()~=36687247 and c:IsAbleToHand()
end
function c36687247.thtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c36687247.filter,tp,LOCATION_DECK,0,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK)
end
function c36687247.thop(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectMatchingCard(tp,c36687247.filter,tp,LOCATION_DECK,0,1,1,nil)
	if g:GetCount()>0 then
		Duel.SendtoHand(g,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,g)
	end
end
