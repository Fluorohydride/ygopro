--宝玉の樹
function c47408488.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--counter
	local e2=Effect.CreateEffect(c)
	e2:SetCategory(CATEGORY_COUNTER)
	e2:SetDescription(aux.Stringid(47408488,0))
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(47408488)
	e2:SetOperation(c47408488.ctop)
	c:RegisterEffect(e2)
	--equip
	local e3=Effect.CreateEffect(c)
	e3:SetCategory(CATEGORY_COUNTER)
	e3:SetDescription(aux.Stringid(47408488,0))
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e3:SetRange(LOCATION_SZONE)
	e3:SetCode(EVENT_EQUIP)
	e3:SetCondition(c47408488.eqcon)
	e3:SetOperation(c47408488.ctop)
	c:RegisterEffect(e3)
	--place
	local e4=Effect.CreateEffect(c)
	e4:SetCategory(CATEGORY_COUNTER)
	e4:SetDescription(aux.Stringid(47408488,1))
	e4:SetType(EFFECT_TYPE_IGNITION)
	e4:SetRange(LOCATION_SZONE)
	e4:SetCost(c47408488.plcost)
	e4:SetTarget(c47408488.pltg)
	e4:SetOperation(c47408488.plop)
	c:RegisterEffect(e4)
end
function c47408488.eqcon(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	return tc:IsFaceup() and tc:IsSetCard(0x1034)
end
function c47408488.ctop(e,tp,eg,ep,ev,re,r,rp)
	e:GetHandler():AddCounter(0x6,1)
end
function c47408488.plcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToGraveAsCost() end
	e:SetLabel(e:GetHandler():GetCounter(0x6))
	Duel.SendtoGrave(e:GetHandler(),REASON_COST)
end
function c47408488.pltg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		local ct=e:GetHandler():GetCounter(0x6)
		return ct>0 and Duel.GetLocationCount(tp,LOCATION_SZONE)>=ct
	end
end
function c47408488.plop(e,tp,eg,ep,ev,re,r,rp)
	local ft=Duel.GetLocationCount(tp,LOCATION_SZONE)
	if ft<=0 then return end
	if ft>e:GetLabel() then ft=e:GetLabel() end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOFIELD)
	local g=Duel.SelectMatchingCard(tp,Card.IsSetCard,tp,LOCATION_DECK,0,ft,ft,nil,0x1034)
	if g:GetCount()>0 then
		local tc=g:GetFirst()
		while tc do
			Duel.MoveToField(tc,tp,tp,LOCATION_SZONE,POS_FACEUP,true)
			local e1=Effect.CreateEffect(e:GetHandler())
			e1:SetCode(EFFECT_CHANGE_TYPE)
			e1:SetType(EFFECT_TYPE_SINGLE)
			e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
			e1:SetReset(RESET_EVENT+0x1fc0000)
			e1:SetValue(TYPE_SPELL+TYPE_CONTINUOUS)
			tc:RegisterEffect(e1)
			tc=g:GetNext()
		end
		Duel.RaiseEvent(g,47408488,e,0,tp,0,0)
	end
end
