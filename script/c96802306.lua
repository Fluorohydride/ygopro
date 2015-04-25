--イグナイト・キャリバー
function c96802306.initial_effect(c)
	--pendulum summon
	aux.AddPendulumProcedure(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--tohand
	local e2=Effect.CreateEffect(c)
	e2:SetCategory(CATEGORY_DESTROY+CATEGORY_TOHAND+CATEGORY_SEARCH)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_PZONE)
	e2:SetCondition(c96802306.thcon)
	e2:SetTarget(c96802306.thtg)
	e2:SetOperation(c96802306.thop)
	c:RegisterEffect(e2)
end
function c96802306.thcon(e,tp,eg,ep,ev,re,r,rp)
	local seq=e:GetHandler():GetSequence()
	local pc=Duel.GetFieldCard(tp,LOCATION_SZONE,13-seq)
	return pc and pc:IsSetCard(0xc8)
end
function c96802306.filter(c)
	return c:IsRace(RACE_WARRIOR) and c:IsAttribute(ATTRIBUTE_FIRE) and c:IsAbleToHand() and not c:IsHasEffect(EFFECT_NECRO_VALLEY)
end
function c96802306.thtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c96802306.filter,tp,LOCATION_DECK+LOCATION_GRAVE,0,1,nil) end
	local pc=Duel.GetFieldCard(tp,LOCATION_SZONE,13-e:GetHandler():GetSequence())
	local g=Group.FromCards(e:GetHandler(),pc)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,2,0,0)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK+LOCATION_GRAVE)
end
function c96802306.thop(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local pc=Duel.GetFieldCard(tp,LOCATION_SZONE,13-e:GetHandler():GetSequence())
	if not pc then return end
	local dg=Group.FromCards(e:GetHandler(),pc)
	if Duel.Destroy(dg,REASON_COST)~=2 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectMatchingCard(tp,c96802306.filter,tp,LOCATION_DECK+LOCATION_GRAVE,0,1,1,nil)
	if g:GetCount()>0 then
		Duel.SendtoHand(g,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,g)
	end
end
