--帝王の開岩
function c61466310.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--spsummon limit
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EFFECT_CANNOT_SPECIAL_SUMMON)
	e2:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e2:SetTargetRange(1,0)
	e2:SetTarget(c61466310.sumlimit)
	c:RegisterEffect(e2)
	--search
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(61466310,0))
	e3:SetCategory(CATEGORY_TOHAND+CATEGORY_SEARCH)
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e3:SetRange(LOCATION_SZONE)
	e3:SetCountLimit(1,61466310)
	e3:SetCode(EVENT_SUMMON_SUCCESS)
	e3:SetCondition(c61466310.thcon)
	e3:SetTarget(c61466310.thtg)
	e3:SetOperation(c61466310.thop)
	c:RegisterEffect(e3)
end
function c61466310.sumlimit(e,c,sump,sumtype,sumpos,targetp)
	return c:IsLocation(LOCATION_EXTRA)
end
function c61466310.thcon(e,tp,eg,ep,ev,re,r,rp)
	return ep==tp and bit.band(eg:GetFirst():GetSummonType(),SUMMON_TYPE_ADVANCE)==SUMMON_TYPE_ADVANCE
end
function c61466310.filter(c,code)
	return (c:GetAttack()==2400 or c:GetAttack()==2800) and c:GetDefence()==1000 and c:GetCode()~=code and c:IsAbleToHand()
end
function c61466310.filter2(c,atk,code)
	return c:GetAttack()==atk and c:GetDefence()==1000 and c:GetCode()~=code and c:IsAbleToHand()
end
function c61466310.thtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsRelateToEffect(e)
		and Duel.IsExistingMatchingCard(c61466310.filter,tp,LOCATION_DECK,0,1,nil,eg:GetFirst():GetCode()) end
	local t1=Duel.IsExistingMatchingCard(c61466310.filter2,tp,LOCATION_DECK,0,1,nil,2400,eg:GetFirst():GetCode())
	local t2=Duel.IsExistingMatchingCard(c61466310.filter2,tp,LOCATION_DECK,0,1,nil,2800,eg:GetFirst():GetCode())
	if t1 and t2 then e:SetLabel(Duel.SelectOption(tp,aux.Stringid(61466310,1),aux.Stringid(61466310,2)))
	elseif t1 then e:SetLabel(Duel.SelectOption(tp,aux.Stringid(61466310,1)))
	else e:SetLabel(Duel.SelectOption(tp,aux.Stringid(61466310,2))+1) end
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK)
	eg:GetFirst():CreateEffectRelation(e)
end
function c61466310.thop(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local ec=eg:GetFirst()
	if ec:IsFacedown() or not ec:IsRelateToEffect(e) then return end
	local atk=e:GetLabel()==0 and 2400 or 2800
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectMatchingCard(tp,c61466310.filter2,tp,LOCATION_DECK,0,1,1,nil,atk,ec:GetCode())
	if g:GetCount()>0 then
		Duel.SendtoHand(g,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,g)
	end
end
