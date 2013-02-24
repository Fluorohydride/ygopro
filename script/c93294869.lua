--傷炎星－ウルブショウ
function c93294869.initial_effect(c)
	--set
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(93294869,0))
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetProperty(EFFECT_FLAG_DELAY+EFFECT_FLAG_DAMAGE_STEP)
	e1:SetCode(EVENT_FLIP)
	e1:SetTarget(c93294869.settg)
	e1:SetOperation(c93294869.setop)
	c:RegisterEffect(e1)
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e2:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e2:SetCode(EVENT_FLIP_SUMMON_SUCCESS)
	e2:SetOperation(c93294869.flipop)
	c:RegisterEffect(e2)
end
function c93294869.filter1(c)
	return c:IsSetCard(0x7c) and c:IsType(TYPE_TRAP) and c:IsSSetable()
end
function c93294869.filter2(c)
	return c:IsSetCard(0x7c) and c:IsType(TYPE_SPELL) and c:IsSSetable()
end
function c93294869.settg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_SZONE)>0
		and Duel.IsExistingMatchingCard(c93294869.filter1,tp,LOCATION_DECK,0,1,nil) end
	if e:GetHandler():GetFlagEffect(93294869)~=0 then
		e:SetLabel(1)
		e:GetHandler():ResetFlagEffect(93294869)
	else
		e:SetLabel(0)
	end
end
function c93294869.setop(e,tp,eg,ep,ev,re,r,rp)
	local ft=Duel.GetLocationCount(tp,LOCATION_SZONE)
	if ft<=0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SET)
	local g=Duel.SelectMatchingCard(tp,c93294869.filter1,tp,LOCATION_DECK,0,1,1,nil)
	local sg=Duel.GetMatchingGroup(c93294869.filter2,tp,LOCATION_DECK,0,nil)
	if e:GetLabel()==1 and sg:GetCount()>0 and Duel.SelectYesNo(tp,aux.Stringid(93294869,1)) then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SET)
		g:AddCard(sg:Select(tp,1,1,nil):GetFirst())
	end
	if g:GetCount()>0 then
		Duel.SSet(tp,g)
		Duel.ConfirmCards(1-tp,g)
	end
end
function c93294869.flipop(e,tp,eg,ep,ev,re,r,rp)
	e:GetHandler():RegisterFlagEffect(93294869,0,0,0)
end
