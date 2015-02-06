--マジック・クロニクル
function c74402414.initial_effect(c)
	local g=Group.CreateGroup()
	g:KeepAlive()
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c74402414.cost)
	e1:SetTarget(c74402414.target)
	e1:SetOperation(c74402414.activate)
	e1:SetLabelObject(g)
	c:RegisterEffect(e1)
	--add counter
	local e0=Effect.CreateEffect(c)
	e0:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_FIELD)
	e0:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e0:SetCode(EVENT_CHAINING)
	e0:SetRange(LOCATION_SZONE)
	e0:SetOperation(aux.chainreg)
	c:RegisterEffect(e0)
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_FIELD)
	e2:SetCode(EVENT_CHAIN_SOLVED)
	e2:SetRange(LOCATION_SZONE)
	e2:SetOperation(c74402414.ctop)
	c:RegisterEffect(e2)
	--salvage
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(74402414,0))
	e3:SetCategory(CATEGORY_TOHAND)
	e3:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e3:SetType(EFFECT_TYPE_IGNITION)
	e3:SetRange(LOCATION_SZONE)
	e3:SetCost(c74402414.thcost)
	e3:SetTarget(c74402414.thtg)
	e3:SetOperation(c74402414.thop)
	e3:SetLabelObject(g)
	c:RegisterEffect(e3)
	--damage
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e4:SetCode(EVENT_LEAVE_FIELD)
	e4:SetCondition(c74402414.damcon)
	e4:SetOperation(c74402414.damop)
	e4:SetLabelObject(g)
	c:RegisterEffect(e4)
end
function c74402414.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	local hg=Duel.GetFieldGroup(tp,LOCATION_HAND,0)
	if chk==0 then return hg:GetCount()>0 and hg:FilterCount(Card.IsAbleToGraveAsCost,nil)==hg:GetCount() end
	Duel.SendtoGrave(hg,REASON_COST)
end
function c74402414.filter(c)
	return c:IsType(TYPE_SPELL+TYPE_TRAP) and c:IsAbleToRemove()
end
function c74402414.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c74402414.filter,tp,LOCATION_DECK,0,5,nil) end
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,nil,5,tp,LOCATION_DECK)
end
function c74402414.activate(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local g=Duel.GetMatchingGroup(c74402414.filter,tp,LOCATION_DECK,0,nil)
	if g:GetCount()<5 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local rg=g:Select(tp,5,5,nil)
	Duel.Remove(rg,POS_FACEUP,REASON_EFFECT)
	local tc=rg:GetFirst()
	while tc do
		tc:RegisterFlagEffect(74402414,RESET_EVENT+0x1fe0000,0,0)
		tc=rg:GetNext()
	end
	e:GetLabelObject():Clear()
	e:GetLabelObject():Merge(rg)
end
function c74402414.ctop(e,tp,eg,ep,ev,re,r,rp)
	if rp~=tp and re:IsHasType(EFFECT_TYPE_ACTIVATE) and re:IsActiveType(TYPE_SPELL) and e:GetHandler():GetFlagEffect(1)>0 then
		e:GetHandler():AddCounter(0x25,1)
	end
end
function c74402414.thcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsCanRemoveCounter(tp,0x25,2,REASON_COST) end
	e:GetHandler():RemoveCounter(tp,0x25,2,REASON_COST)
end
function c74402414.thfilter(c)
	return c:GetFlagEffect(74402414)~=0 and c:IsAbleToHand()
end
function c74402414.thtg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return e:GetLabelObject():IsContains(chkc) and c74402414.thfilter(chkc) end
	if chk==0 then return e:GetLabelObject():IsExists(c74402414.thfilter,1,nil) end
	Duel.Hint(HINT_SELECTMSG,1-tp,HINTMSG_ATOHAND)
	local g=e:GetLabelObject():FilterSelect(1-tp,c74402414.thfilter,1,1,nil)
	e:GetLabelObject():Sub(g)
	Duel.SetTargetCard(g)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_REMOVED)
end
function c74402414.thop(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.SendtoHand(tc,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,tc)
	end
end
function c74402414.dfilter(c)
	return c:GetFlagEffect(74402414)~=0
end
function c74402414.damcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():IsStatus(STATUS_ACTIVATED)
end
function c74402414.damop(e,tp,eg,ep,ev,re,r,rp)
	local ct=e:GetLabelObject():FilterCount(c74402414.dfilter,nil)
	Duel.Damage(tp,ct*500,REASON_EFFECT)
end
